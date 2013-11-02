// EPOS UDP Protocol Implementation

#include <udp.h>

__BEGIN_SYS

// Class attributes
UDP::List UDP::_received;


// Methods
int UDP::send(const Address & from, const Address & to, const void * data, unsigned int size)
{
    db<UDP>(TRC) << "UDP::send(f=" << from << ",t=" << to << ",d=" << data << ",s=" << size << ")" << endl;

    unsigned int ret = (size > sizeof(Data)) ? sizeof(Data) : size;

    NIC::Buffer * pool = _ip->nic()->alloc(sizeof(Header), sizeof(IP::Header), ret);

    NIC::Buffer * buf = pool;
    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());
    Message * message = reinterpret_cast<Message *>(packet->data());

    db<UDP>(INF) << "UDP::send:msg=" << message << " => " << *message << endl;

    const unsigned char * src = reinterpret_cast<const unsigned char *>(data);
    unsigned char * dst = message->data();
    unsigned int length = IP::MAX_FRAGMENT - sizeof(Header);

    new(packet->data()) Header(from.port(), to.port(), size);
    message->sum(data);

    do {
        memcpy(dst, src, length);

        buf = buf->next();
        packet = reinterpret_cast<Packet *>(buf->frame()->data());

        size -= length;
        src += length;
        dst = packet->data();
        length = (size > IP::MAX_FRAGMENT) ? IP::MAX_FRAGMENT : size;
    } while(buf);

    _ip->send(to.ip(), IP::UDP, pool);

    return ret;
}


int UDP::receive(NIC::Buffer * buf, void * d, unsigned int s)
{
    unsigned char * data = reinterpret_cast<unsigned char *>(d);

    db<UDP>(TRC) << "UDP::receive(buf=" << buf << ",d=" << d << ",s=" << s << ")" << endl;

    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());
    Message * message = reinterpret_cast<Message *>(packet->data());

    unsigned int size =  message->length() - sizeof(Header);
    if(size > s)
        size = s;
    unsigned int ret = size;

    unsigned int frag = packet->length() - sizeof(IP::Header) - sizeof(Header);
    if(frag > size)
        frag = size;

    memcpy(data, message->data(), frag);
    size -= frag;

    for(buf = buf->next(); buf && (size > 0); buf = buf->next()) { // Fragmented
        packet = reinterpret_cast<Packet *>(buf->frame()->data());
        frag = packet->length() - sizeof(IP::Header);
        if(frag > size)
            frag = size;
        memcpy(&data[packet->offset() - sizeof(Header)], packet->data(), frag);
        size -= frag;

        _ip->nic()->free(buf);
    }

    return ret;
}


void UDP::update(IP::Observed * ip, int port, NIC::Buffer * buf)
{
    db<UDP>(TRC) << "UDP::update(buf=" << buf << ")" << endl;

    Packet * packet = reinterpret_cast<Packet *>(buf->frame()->data());
    Message * message = reinterpret_cast<Message *>(packet->data());

    db<UDP>(INF) << "UDP::update:msg=" << message << " => " << *message << endl;

    if(!message->check()) {
        db<UDP>(WRN) << "UDP::update: wrong message checksum!" << endl;
        return;
    }

    notify(message->to_port(), buf);
}


unsigned short UDP::checksum(const void * header, const void * data, unsigned int size)
{
    unsigned long sum = IP::checksum(header, sizeof(Pseudo_Header));
    sum += IP::checksum(data, size);

    while(sum >> 16)
        sum = (sum & 0xffff) + (sum >> 16);

    return ~sum;
}

__END_SYS

