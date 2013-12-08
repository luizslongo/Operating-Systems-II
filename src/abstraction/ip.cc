// EPOS IP Protocol Implementation

#include <utility/string.h>
#include <arp.h>
#include <ip.h>
#include <udp.h>
#include <dhcp.h>

__BEGIN_SYS

// Class attributes
unsigned short IP::Header::_next_id = 0;
IP::Router IP::_router;
IP::Fragmented IP::_fragmented;
IP::Observed IP::_observed;


// Methods
void IP::config_by_info()
{
    _address = System::info()->bm.node_id;

    if(!_address)
        db<IP>(WRN) << "IP::config_by_info: no valid address found in System_Info!" << endl;
}

void IP::config_by_dhcp()
{
    db<IP>(TRC) << "IP::config_by_dhcp()" << endl;

    _arp.insert(Address::BROADCAST, _nic.broadcast());
    _router.insert(&_nic, this, &_arp, Address::NULL, Address::BROADCAST, Address::NULL);
    DHCP::Client(_nic.address(), this);
    _router.remove(Address::BROADCAST);
    _arp.remove(Address::BROADCAST);

    db<IP>(TRC) << "IP::config_by_dhcp() => " << *this << endl;
}


IP::~IP()
{
    _nic.detach(this, NIC::IP);
}


IP::Buffer * IP::alloc(const Address & to, const Protocol & prot, unsigned int once, unsigned int payload)
{
    db<IP>(TRC) << "IP::alloc(to=" << to << ",prot=" << prot << ",on=" << once<< ",pl=" << payload << ")" << endl;

    Route * through = _router.search(to);
    IP * ip = through->ip();
    NIC * nic = through->nic();

    MAC_Address mac = through->arp()->resolve((through->gateway() == through->ip()->address()) ? to : through->gateway());
    if(!mac) {
         db<IP>(WRN) << "IP::alloc: destination host (" << to << ") unreachable!" << endl;
         return 0;
    }

    Buffer * pool = nic->alloc(nic, mac, NIC::IP, once, sizeof(IP::Header), payload);

    Header header(ip->address(), to, prot, 0); // length will be defined latter for each fragment

    unsigned int offset = 0;
    for(Buffer::Element * el = &pool->link(); el; el = el->next()) {
        Packet * packet = el->object()->frame()->data<Packet>();

        // Setup header
        memcpy(packet->header(), &header, sizeof(Header));
        packet->flags(el->next() ? Header::MF : 0);
        packet->length(el->object()->size());
        packet->offset(offset);
        packet->header()->sum();
        db<IP>(INF) << "IP::alloc:pkt=" << packet << " => " << *packet << endl;

        offset += MAX_FRAGMENT;
    }

    return pool;
}


int IP::send(Buffer * buf)
{
    db<IP>(TRC) << "IP::send(buf=" << buf << ")" << endl;

    return buf->nic()->send(buf); // implicitly releases the pool
}


void IP::update(NIC::Observed * nic, int prot, Buffer * buf)
{
    db<IP>(TRC) << "IP::update(nic=" << nic << ",prot=" << hex << prot << dec << ",buf=" << buf << ")" << endl;

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
            if(!notify(packet->protocol(), buf))
                buf->nic()->free(buf);
        }
    } else {
        db<IP>(INF) << "IP::update: notify whole datagram" << endl;
        if(!notify(packet->protocol(), buf))
            buf->nic()->free(buf);
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
