// EPOS UDP Protocol Implementation

#include <udp.h>

__BEGIN_SYS

// Class attributes
UDP::List UDP::_received;


// Methods
int UDP::send(const Port & from, const Address & to, const void * d, unsigned int s)
{
    const unsigned char * data = reinterpret_cast<const unsigned char *>(d);
    unsigned int size = (s > sizeof(Data)) ? sizeof(Data) : s;

    db<UDP>(TRC) << "UDP::send(f=" << from << ",t=" << to << ",d=" << data << ",s=" << size << ")" << endl;

    Buffer * pool = IP::route(to.ip())->nic()->alloc(sizeof(Header), sizeof(IP::Header), size);

    for(Buffer::Element * el = &pool->link(); el; el = el->next()) {
        Buffer * buf = el->object();
        Packet * packet = buf->frame()->data<Packet>();

        db<UDP>(INF) << "UDP::send:buf=" << buf << " => " << *buf<< endl;

        if(el == &pool->link()) {
            Message * message = packet->data<Message>();
            new(packet->data<void>()) Header(from, to.port(), size);
            message->sum(data);
            memcpy(message->data<void>(), data, buf->size() - sizeof(Header) - sizeof(IP::Header));
            data += buf->size() - sizeof(Header) - sizeof(IP::Header);

            db<UDP>(INF) << "UDP::send:msg=" << message << " => " << *message << endl;
        } else {
            memcpy(packet->data<void>(), data, buf->size() - sizeof(IP::Header));
            data += buf->size() - sizeof(IP::Header);
        }
    }

    return IP::send(to.ip(), IP::UDP, pool); // Calls nic->free(&pool)
}


int UDP::receive(Buffer * buf, void * d, unsigned int s)
{
    for(Buffer::Element * el = &buf->link(); el; el = el->next()) {
        Buffer * b = el->object();
        db<PCNet32>(INF) << "PCNet32::alloc:buf=" << b << " => " << *b << endl;
    }

    unsigned char * data = reinterpret_cast<unsigned char *>(d);

    db<UDP>(TRC) << "UDP::receive(buf=" << buf << ",d=" << d << ",s=" << s << ")" << endl;

    Buffer::Element * head = &buf->link();
    unsigned int size = 0;

    for(Buffer::Element * el = head; el && (size <= s); el = el->next()) {
        db<UDP>(INF) << "UDP::receive:buf=" << buf << " => " << *buf << endl;

        Buffer * buf = el->object();
        Packet * packet = buf->frame()->data<Packet>();

        unsigned int len = buf->size() - sizeof(IP::Header);
        if(el == head) {
            Message * message = packet->data<Message>();
            len -= sizeof(Header);
            memcpy(data, message->data<void>(), len);

            db<UDP>(INF) << "UDP::receive:msg=" << message << " => " << *message << endl;
        } else
            memcpy(data, packet->data<void>(), len);

        db<UDP>(INF) << "UDP::receive:len=" << len << endl;

        data += len;
        size += len;

        buf->nic()->free(buf);
    }

    return size;
}


void UDP::update(IP::Observed * ip, int port, Buffer * buf)
{
    db<UDP>(TRC) << "UDP::update(buf=" << buf << ")" << endl;

    Packet * packet = buf->frame()->data<Packet>();
    Message * message = packet->data<Message>();

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

