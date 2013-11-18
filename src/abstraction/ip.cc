// EPOS IP Protocol Implementation

#include <utility/string.h>
#include <ip.h>

__BEGIN_SYS

// Class attributes
unsigned short IP::Header::_next_id = 0;
IP::Route::Table IP::Route::_table;
IP::ARP::Hash IP::ARP::_table;
IP::Fragmented IP::_fragmented;
IP::Observed IP::_observed;


// Methods
IP::IP()
{
    if(Traits<IP>::CONFIG == Traits<IP>::STATIC) {
        _address = Traits<IP>::ADDRESS;
        _netmask = Traits<IP>::NETMASK;
        _broadcast = Traits<IP>::BROADCAST;
        _gateway = Traits<IP>::GATEWAY;
    } else
        ; // DHCP

        new (SYSTEM) Route(Traits<IP>::ADDRESS, &_nic, Traits<IP>::ADDRESS & Traits<IP>::NETMASK, Traits<IP>::ADDRESS, Traits<IP>::NETMASK);
        new (SYSTEM) Route(Traits<IP>::ADDRESS, &_nic, 0UL, Traits<IP>::GATEWAY, 0UL); // Default route must be the last one in table

    new (SYSTEM) ARP(&_nic, Traits<IP>::ADDRESS, _nic.broadcast());
    new (SYSTEM) ARP(&_nic, Traits<IP>::GATEWAY, _nic.broadcast());

    _nic.attach(this, NIC::IP);
}


int IP::send(const Address & to, const Protocol & prot, Buffer * buf)
{
    db<IP>(TRC) << "IP::send(to=" << to << ",prot=" << prot << ",buf=" << buf << ")" << endl;

    const Route * through = route(to);
    db<IP>(INF) << "IP::send:route=" << through << " => " << *through << endl;

    NIC * nic = through->nic();
    MAC_Address mac = arp(nic, through->gateway());
    db<IP>(INF) << "IP::ARP(to=" << to << ") => " << mac << endl;

    Header header(through->from(), to, prot, 0); // length will be defined latter for each fragment

    unsigned int offset = 0;
    for(Buffer::Element * el = &buf->link(); el; el = el->next()) {
        Packet * packet = el->object()->frame()->data<Packet>();

        // Setup header
        memcpy(packet->header(), &header, sizeof(Header));
        packet->flags(el->next() ? Header::MF : 0);
        packet->length(buf->size());
        packet->offset(offset);
        packet->header()->sum();
        db<IP>(INF) << "IP::send:pkt=" << packet << " => " << *packet << endl;

        offset += MAX_FRAGMENT;
    }

    return nic->send(mac, NIC::IP, buf); // Release pool
}


void IP::update(NIC::Observed * nic, int prot, Buffer * buf)
{
    db<IP>(TRC) << "IP::update(nic=" << nic << ",prot=" << prot << ",buf=" << buf << ")" << endl;

    Packet * packet = buf->frame()->data<Packet>();

    db<IP>(INF) << "IP::update:pkt=" << packet << " => " << *packet << endl;

    if(!packet->check()) {
        db<IP>(WRN) << "IP::update: wrong packet checksum!" << endl;
        _nic.free(buf);
        return;
    }

    unsigned long key = (packet->protocol() << 24) | (packet->id() << 8);
    buf->nic(&_nic);

    if((packet->flags() & Header::MF) || (packet->offset() != 0)) { // Fragment
        Element * el = _fragmented.search_key(key);
        if(!el) {
            el = new (SYSTEM) Fragmented::Element(new (SYSTEM) Buffer::List, key);
            _fragmented.insert(el);
        }
        el->object()->insert(&buf->link());

        // Sum the lengths of the received fragments and find the last element
        unsigned int total = 0;
        unsigned int accumulated = 0;
        for(Buffer::Element * el2 = el->object()->head(); el2; el2 = el2->next()) {
            db<IP>(INF) << "IP::update:buf=" << el2->object() << " => " << *el2->object() << endl;

            packet = el2->object()->frame()->data<Packet>();
            if(!(packet->flags() & Header::MF))
                total = packet->offset() + packet->length() - sizeof(Header);
            accumulated += packet->length() - sizeof(Header);
        }

        // If the datagram is complete, report it to the upper layer
        if(accumulated == total) {
            db<IP>(INF) << "IP::update: notify fragmented datagram" << endl;
            buf = el->object()->head()->object();
            _fragmented.remove(el);
            delete el->object();
            delete el;
            notify(packet->protocol(), buf);
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
