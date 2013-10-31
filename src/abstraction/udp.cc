// EPOS UDP Protocol Implementation

#include <udp.h>

__BEGIN_SYS

// Class attributes
UDP::List UDP::_received;


// Methods
int UDP::send(const Address & from, const Address & to, const void * data, unsigned int size)
{
    db<UDP>(TRC) << "UDP::send(f=" << from << ",t=" << to << ",d=" << data << ",s=" << size << ")" << endl;

    Message message(from, to, data, size);

    db<UDP>(INF) << "UDP::send([msg=" << &message << "]) => " << message << endl;

    return _ip->send(to.ip(), IP::UDP, &message, message.length()) - sizeof(Header);
}

//int UDP::receive(Address * from, void * data, unsigned int size)
//{
//    db<UDP>(TRC) << "UDP::receive(f=" << *from << ",d=" << data << ",s=" << size << ")" << endl;
//
//    Element * el = _received.remove();
//    NIC::Buffer * buf = reinterpret_cast<NIC::Buffer *>(el);
//    IP::Packet * packet = reinterpret_cast<IP::Packet *>(buf->frame()->data());
//    Message * message = reinterpret_cast<Message *>(packet->data());
//
//    *from = Address(packet->from(), message->from_port());
//
//    if(size > message->length() - sizeof(IP::Header) - sizeof(Header))
//        size = message->length() - sizeof(IP::Header) - sizeof(Header);
//
//    if((packet->flags() & IP::Header::MF) || (packet->offset() != 0)) // Fragmented
//        while(el) {
//            buf = reinterpret_cast<NIC::Buffer *>(el);
//            packet = reinterpret_cast<IP::Packet *>(buf->frame()->data());
//            message = reinterpret_cast<Message *>(packet->data());
//
//            memcpy((char *)data + packet->offset() - sizeof(Header), message->data(), message->length() - sizeof(Header)); // Fragmentation still to be handled
//
//            _ip->nic()->received(buf);
//            el = reinterpret_cast<Element *>(el->object());
//        }
//    else {
//        memcpy(data, message->data(), size);
//        _ip->nic()->received(buf);
//    }
//
//    return size;
//}


int UDP::receive(NIC::Buffer * buf, void * data, unsigned int size)
{
    db<UDP>(TRC) << "UDP::receive(buf=" << buf << ",d=" << data << ",s=" << size << ")" << endl;

    Element * el = reinterpret_cast<Element *>(buf);
    IP::Packet * packet = reinterpret_cast<IP::Packet *>(buf->frame()->data());
    Message * message = reinterpret_cast<Message *>(packet->data());

    if(size > message->length() - sizeof(Header))
        size = message->length() - sizeof(Header);

    if((packet->flags() & IP::Header::MF) || (packet->offset() != 0)) { // Fragmented
        unsigned char * ptr = reinterpret_cast<unsigned char *>(data);
        while(el) {
            buf = reinterpret_cast<NIC::Buffer *>(el);
            packet = reinterpret_cast<IP::Packet *>(buf->frame()->data());

            if(packet->offset())
                memcpy(&ptr[packet->offset() - sizeof(Header)], packet->data(), packet->length() - sizeof(IP::Header));
            else
                memcpy(ptr, message->data(), packet->length() - sizeof(IP::Header) - sizeof(Header));

            _ip->nic()->received(buf);
            el = reinterpret_cast<Element *>(el->object());
        }
    } else {
        memcpy(data, message->data(), size);
        _ip->nic()->received(buf);
    }

    return size;
}


void UDP::update(IP::Observed * ip, int port, NIC::Buffer * buf)
{
    db<UDP>(TRC) << "UDP::update(buf=" << buf << ")" << endl;

    IP::Packet * packet = reinterpret_cast<IP::Packet *>(buf->frame()->data());
    Message * message = reinterpret_cast<Message *>(packet->data());

    db<UDP>(INF) << "UDP::received(msg=" << message << ") => " << *message << endl;

    if(!message->check(packet->from(), packet->to())) {
        db<UDP>(WRN) << "UDP::received: wrong message checksum!" << endl;
        return;
    }

    notify(message->to_port(), buf);
}

__END_SYS

