// EPOS IP Protocol Implementation

#include <utility/string.h>
#include <arp.h>
#include <ip.h>
#include <udp.h>
#include <dhcp.h>

__BEGIN_SYS

// Class attributes
unsigned short IP::Header::_next_id = 0;
IP * IP::_networks[];
IP::Router IP::_router;
IP::Reassembling IP::_reassembling;
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
    for(Buffer::Element * el = pool->link(); el; el = el->next()) {
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

    buf->nic(&_nic);

    unsigned long key = (packet->protocol() << 24) | (packet->id() << 8);

    if((packet->flags() & Header::MF) || (packet->offset() != 0)) { // Fragmented
        Fragmented * frag;
        Reassembling::Element * el = _reassembling.search_key(key);

        if(el)
            frag = el->object();
        else {
            frag = new (SYSTEM) Fragmented(key);
            _reassembling.insert(frag->link());
        }

        frag->insert(buf);

        if(frag->reassembled()) {
            db<IP>(INF) << "IP::update: notifying reassembled datagram" << endl;
            buf = frag->pool();
            _reassembling.remove(frag->link());
            delete frag;
            if(!notify(packet->protocol(), buf))
                buf->nic()->free(buf);
        }
    } else {
        db<IP>(INF) << "IP::update: notifying whole datagram" << endl;
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
