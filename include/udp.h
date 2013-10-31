// EPOS UDP Protocol Declarations

#ifndef __udp_h
#define __udp_h

#include <ip.h>

__BEGIN_SYS 

class UDP: private IP::Observer, public Data_Observed<NIC::Buffer>
{
    template<typename Channel, typename Network> friend class Socket;

private:
    // List to hold received NIC::Buffers
    typedef Ordered_List<NIC::Buffer, unsigned long> List;
    typedef List::Element Element;

    // Pseudo header for checksum calculations
    struct Pseudo_Header {
        unsigned long from;
        unsigned long to;
        unsigned char zero;
        unsigned char protocol;
        unsigned short length;

        Pseudo_Header(unsigned long f, unsigned long t, unsigned short l)
        : from(f), to(t), zero(0), protocol(IP::UDP), length(htons(l)) {};
    };

public:
    typedef IP Network;

    typedef unsigned short Port;
    
    class Address
    {
    public:
        typedef Port Local;

    public:
        Address() {}
        Address(const IP::Address & ip, const Port & port): _ip(ip), _port(port) {}
        Address(const char * addr): _ip(addr) { // a.b.c.d:port
            char * sep = strchr(addr, ':');
            _port = sep ? atol(++sep) : 0;
        }

        const IP::Address & ip() const { return _ip; }
        const Port & port() const { return _port; }
        const Local & local() const { return _port; }

        bool operator==(const Address & a) {
            return (_ip == a._ip) && (_port == a._port);
        }

        friend OStream & operator<<(OStream & db, const Address & a) {
            db << a._ip << ":" << a._port;
            return db;
        }

    private:
        IP::Address _ip;
        Port _port;
    };
    
    typedef Data_Observer<NIC::Buffer> Observer;
    typedef Data_Observed<NIC::Buffer> Observed;

    class Header
    {
    public:
        Header() {}
        Header(const Port & from, const Port & to, unsigned int size):
            _from_port(htons(from)), _to_port(htons(to)), _length(htons(size)), _checksum(0) {}

        Port from_port() const { return ntohs(_from_port); }
        Port to_port() const { return ntohs(_to_port); }

        unsigned short length() const { return ntohs(_length); }

        unsigned short checksum() const { return ntohs(_checksum); }

        friend OStream & operator<<(OStream & db, const Header & h) {
            db << "{sp=" << ntohs(h._from_port) << ",dp=" << ntohs(h._to_port)
               << ",len=" << ntohs(h._length) << ",chk=" << hex << ntohs(h._checksum) << dec << "}";
            return db;
        }

    protected:
        unsigned short _from_port;
        unsigned short _to_port;
        unsigned short _length; // Length of datagram (header + data) in bytes
        unsigned short _checksum; // Pseudo header checksum (see RFC)
    };

    static const unsigned int MTU = 65536 - sizeof(Header) - sizeof(IP::Header);
    typedef unsigned char Data[MTU];

    class Message: public Header
    {
    public:
        Message() {}
        Message(const Address & from, const Address & to, const void * data, unsigned int size)
        : Header(from.port(), to.port(), (size > sizeof(Data) ? sizeof(Data) : size) + sizeof(Header)) {
            memcpy(_data, data, size > sizeof(Data) ? sizeof(Data) : size);
            _checksum = Traits<UDP>::checksum ? sum(from.ip(), to.ip()) : 0;
        }

        Header * header() { return this; }
        unsigned char * data() { return _data; }

        bool check(const IP::Address & from, const IP::Address & to) const {
            return Traits<UDP>::checksum ? (_checksum == sum(from, to)) : true;
        }

        friend Debug & operator<<(Debug & db, const Message & m) {
            db << "{head=" << reinterpret_cast<const Header &>(m) << ",data=" << m._data << "}";
            return db;
        }

    private:
        unsigned short sum(const IP::Address & from, const IP::Address & to) const {
            Pseudo_Header pseudo(from, to, length());

            unsigned long sum = IP::checksum(&pseudo, sizeof(Pseudo_Header));
            sum += IP::checksum(this, length());

            while(sum >> 16)
                sum = (sum & 0xffff) + (sum >> 16);

            return ~sum;
        }

    private:
        Data _data;
    } __attribute__((packed, may_alias));

public:
    UDP(IP * ip): _ip(ip) { _ip->attach(this, IP::UDP); }
    ~UDP() { _ip->detach(this, IP::UDP); }

    int send(const Address & from, const Address & to, const void * data, unsigned int size);
    int receive(Address * from, void * data, unsigned int size);

private:
    void update(IP::Observed * ip, int port, NIC::Buffer * buf);

    int receive(NIC::Buffer * buf, void * data, unsigned int size);

private:
    IP * _ip;

    static List _received;
};

__END_SYS

#endif
