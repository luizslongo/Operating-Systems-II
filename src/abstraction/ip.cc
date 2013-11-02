// EPOS IP Protocol Implementation

#include <utility/string.h>
#include <ip.h>
#include <alarm.h>

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
    } else
        ; // DHCP

    new (SYSTEM) Route(_nic, static_cast<unsigned long>(_address) & static_cast<unsigned long>(_netmask), _address, _netmask);
    new (SYSTEM) Route(_nic, 0UL, _gateway, 0UL); // Default route must be the last one in table

    new (SYSTEM) ARP(_nic, _address, _nic->broadcast());
    new (SYSTEM) ARP(_nic, _gateway, _nic->broadcast());

    _nic->attach(this, NIC::IP);
}


int IP::send(const Address & to, const Protocol & prot, Buffer * buf)
{
    db<IP>(TRC) << "IP::send(to=" << to << ",prot=" << prot << ",buf=" << buf << ",size=" << buf->size() << ")" << endl;

    Header header(_address, to, prot, 0); // length will be defined latter for each fragment
    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());
    db<IP>(INF) << "IP::send:pkt=" << packet << " => " << *packet << endl;

    const Route * through = route(to);
    db<IP>(INF) << "IP::send:route=" << through << " => " << *through << endl;

    NIC * nic = through->nic();
    MAC_Address mac = arp(nic, through->gateway());
    db<IP>(INF) << "IP::send:dst=" << mac << endl;

    unsigned int ret = buf->size();

    int size = ret;
    int offset = 0;
    while(size > 0) {
        Buffer * next = buf->next();

        memcpy(packet->header(), &header, sizeof(Header));

        if(size > int(MAX_FRAGMENT)) {
            packet->length(MAX_FRAGMENT + sizeof(Header));
            packet->flags(Header::MF);
        } else {
            packet->length(size + sizeof(Header));
            packet->flags(0);
        }

        packet->offset(offset);
        packet->header()->sum();
        buf->size(packet->length());
        nic->send(mac, NIC::IP, buf); // Release buffer
        Delay(100000);

        offset += MAX_FRAGMENT;
        size -= MAX_FRAGMENT;

        buf = next;
        packet = reinterpret_cast<Packet *>(buf->frame()->data());
    }

    return ret;
}


void IP::update(NIC::Observed * nic, int prot, Buffer * buf)
{
    db<IP>(TRC) << "IP::update(nic=" << nic << ",prot=" << prot << ",buf=" << buf << ")" << endl;

    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());

    db<IP>(INF) << "IP::update:pkt=" << packet << " => " << *packet << endl;

    if(!packet->check()) {
        db<IP>(WRN) << "IP::update: wrong packet checksum!" << endl;
        _nic->free(buf);
        return;
    }

    unsigned long key = (packet->protocol() << 16) | packet->id();
    buf->link(Element(0, key));

    if((packet->flags() & Header::MF) || (packet->offset() != 0)) { // Fragment
        Element * head = _fragmented.search_rank(key);
        if(!head) // First fragment of this datagram
            _fragmented.insert(buf->link());
        else {
            // Sum the lengths of the received fragments and find the last element
            unsigned int total = 0;
            unsigned int accumulated = 0;
            Buffer * current, * last;
            for(current = reinterpret_cast<Buffer *>(head); current; last = current, current = current->next()) {
                packet = reinterpret_cast<Packet *>(current->frame()->data());
                if(!(packet->flags() & Header::MF))
                    total = packet->offset() + packet->length() - sizeof(Header);
                accumulated += packet->length() - sizeof(Header);
            }

            // Insert fragment at the end of the list
            last->next(buf);

            // Add this fragment
            packet = reinterpret_cast<Packet *>(buf->frame()->data());
            if(!(packet->flags() & Header::MF))
                total = packet->offset() + packet->length() - sizeof(Header);
            accumulated += packet->length() - sizeof(Header);

            // If the datagram is complete, report it to the upper layer
            if(accumulated == total) {
                db<IP>(INF) << "IP::update: notify fragmented datagram" << endl;
                _fragmented.remove(head);
                buf = reinterpret_cast<Buffer *>(head);
                notify(packet->protocol(), buf);
            }
        }
    } else {
        db<IP>(INF) << "IP::update: notify whole datagram" << endl;
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
