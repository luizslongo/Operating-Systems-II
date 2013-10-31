// EPOS IP Protocol Implementation

#include <thread.h>
#include <ip.h>

__BEGIN_SYS

// Class attributes
unsigned short IP::Header::_next_id = 0;
IP::Route::Table IP::Route::_table;
IP::ARP::Hash IP::ARP::_table;
IP::List IP::_fragmented;


// Methods
IP::IP(NIC * nic): _nic(nic)
{
    if(Traits<IP>::CONFIG == Traits<IP>::STATIC) {
        _address = Traits<IP>::ADDRESS;
        _netmask = Traits<IP>::NETMASK;
        _broadcast = Traits<IP>::BROADCAST;
        _gateway = Traits<IP>::GATEWAY;
    }
    //    else DHCP

    new (SYSTEM) Route(_nic, static_cast<unsigned long>(_address) & static_cast<unsigned long>(_netmask), _address, _netmask);
    new (SYSTEM) Route(_nic, 0UL, _gateway, 0UL);

    new (SYSTEM) ARP(_nic, _address, _nic->BROADCAST);
    new (SYSTEM) ARP(_nic, _gateway, _nic->BROADCAST);

    _nic->attach(this, NIC::IP);
}


int IP::send(const Address & to, const Protocol & prot, const void * data, unsigned int size)
{
    db<IP>(TRC) << "IP::send(f=" << _address <<",t=" << to << ",p=" << prot << ",d=" << data << ",s=" << size << ")" << endl;

    Packet packet(_address, to, prot, data, size);
    db<IP>(INF) << "IP::send([pkt=" << &packet << "]) => " << packet << endl;

    const Route * through = route(to);
    db<IP>(INF) << "IP::send([route=" << through << " => " << *through << endl;

    NIC * nic = through->nic();
    MAC_Address mac = arp(through->gateway());

    if(size <= MAX_FRAGMENT)
        _nic->send(mac, NIC::IP, &packet, packet.length());
    else {
        unsigned int offset = 0;
        const unsigned char * ptr = reinterpret_cast<const unsigned char *>(data);
        while(int(size) >= 0) {
            if(size > MAX_FRAGMENT) {
                memcpy(packet.data(), ptr, MAX_FRAGMENT);
                packet.length(MAX_FRAGMENT + sizeof(Header));
                packet.offset(offset);
                packet.flags(Header::MF);
                packet.header()->sum();
                nic->send(mac, NIC::IP, &packet, packet.length());
            } else {
                memcpy(packet.data(), ptr, size);
                packet.length(size + sizeof(Header));
                packet.offset(offset);
                packet.flags(0);
                packet.header()->sum();
                nic->send(mac, NIC::IP, &packet, packet.length());
            }
            offset += MAX_FRAGMENT;
            ptr += MAX_FRAGMENT;
            size -= MAX_FRAGMENT;
        }
    }

    return size;
}


//int IP::receive(Address * from, Protocol * protocol, void * data, unsigned int size)
//{
//    db<IP>(TRC) << "IP::receive(f=" << from << ",p=" << *protocol << ",d=" << data << ",s=" << size << ")" << endl;
//
//    Element * el = 0;
//    for(; !el; Thread::yield())
//        if(protocol && *protocol)
//            el = _received.remove_rank(*protocol);
//        else
//            el = _received.remove();
//
//    NIC::Buffer * buf = reinterpret_cast<NIC::Buffer *>(el);
//    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());
//
//    db<IP>(INF) << "IP::receive(...) => " << *packet << endl;
//
//    *from = packet->from();
//    *protocol = packet->protocol();
//
//    if((packet->flags() & Header::MF) || (packet->offset() != 0)) { // Fragmented
//        unsigned int copied = 0;
//        for(; el; el = el->object()->link<Element>()) {
//            buf = reinterpret_cast<NIC::Buffer *>(el);
//            packet = reinterpret_cast<Packet *>(buf->frame()->data());
//            copied += packet->length() - sizeof(Header);
//            if(copied > size)
//                break;
//            memcpy(&reinterpret_cast<char *>(data)[packet->offset()], packet->data(), packet->length());
//            _nic->received(buf);
//        }
//        size = copied;
//    } else {
//        if(size > packet->length() - sizeof(Header))
//            size = packet->length() - sizeof(Header);
//        memcpy(data, packet->data(), size);
//        _nic->received(buf);
//    }
//
//    return size;
//}


void IP::update(NIC::Observed * nic, int prot, NIC::Buffer * buf)
{
    db<IP>(TRC) << "IP::update(nic=" << nic << ",prot=" << prot << ",buf=" << buf << ")" << endl;

    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());

    db<IP>(INF) << "IP::received(...) => pkt[" << packet << "]=" << *packet << endl;

    if(!packet->check()) {
        db<IP>(WRN) << "IP::received: wrong packet checksum!" << endl;
        _nic->received(buf);
        return;
    }

    unsigned long key = (packet->protocol() << 16) | packet->id();
    buf->link<Element>(Element(0, key));

    if((packet->flags() & Header::MF) || (packet->offset() != 0)) { // Fragment
        Element * head = _fragmented.search_rank(key);
        if(!head) // First fragment of this datagram
            _fragmented.insert(buf->link<Element>());
        else {
            // Insert fragment at the end of the list pointed by object()
            Element * el;
            for(el = head; el->object(); el = el->object()->link<Element>());
            *el = Element(buf, key);

            // Check if the fragment completes a datagram
            unsigned int total = 0;
            unsigned int current = 0;
            for(el = head; el; el = el->object()->link<Element>()) {
                buf = reinterpret_cast<NIC::Buffer *>(el);
                packet = reinterpret_cast<Packet *>(buf->frame()->data());
                if(!(packet->flags() & Header::MF))
                    total = packet->offset() + packet->length() - sizeof(Header);
                current += packet->length() - sizeof(Header);
            }
            if(current == total) {
                db<IP>(INF) << "IP::update() => notify fragmented datagram" << endl;
                _fragmented.remove(head);
                buf = reinterpret_cast<NIC::Buffer *>(head);
                notify(packet->protocol(), buf);
            }
        }
    } else {
        db<IP>(INF) << "IP::update() => notify whole datagram" << endl;
        notify(packet->protocol(), buf);
    }
}


unsigned short IP::checksum(const void * data, unsigned int size)
{
    db<IP>(TRC) << "IP::checksum(d=" << data << ",s=" << size << ")" << endl;

    const unsigned char * ptr = reinterpret_cast<const unsigned char *>(data);
    unsigned long sum = 0;

    for(unsigned int i = 0; i < size - 1; i += 2)
        sum += (ptr[i] << 8) | ptr[i+1];

    if(size & 1)
        sum += ptr[size - 1];

    while(sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

__END_SYS
