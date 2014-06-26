// EPOS UDP Protocol Declarations

#ifndef __udp_h
#define __udp_h

#include <ip.h>

__BEGIN_SYS 

class UDP: private IP::Observer, public Data_Observed<NIC::Buffer>
{
private:
    // List to hold received Buffers
    typedef NIC::Buffer Buffer;
    typedef Buffer::List List;
    typedef List::Element Element;

    // IP Packet
    typedef IP::Packet Packet;

    // Pseudo IP header for checksum calculations
    class Pseudo_Header
    {
    public:
        Pseudo_Header(const IP::Address & f, const IP::Address & t, unsigned short l):
            _from(f), _to(t), _zero(0), _protocol(IP::UDP), _length(htons(l)) {};

    private:
        IP::Address _from;
        IP::Address _to;
        unsigned char _zero;
        unsigned char _protocol;
        unsigned short _length;
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
            char * token = strchr(addr, ':');
            _port = token ? atol(++token) : 0;
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
            _from(htons(from)), _to(htons(to)), _length(htons((size > sizeof(Data) ? sizeof(Data) : size) + sizeof(Header))) {}

        Port from() const { return ntohs(_from); }
        Port to() const { return ntohs(_to); }

        unsigned short length() const { return ntohs(_length); }

        unsigned short checksum() const { return ntohs(_checksum); }

        friend OStream & operator<<(OStream & db, const Header & h) {
            db << "{sp=" << ntohs(h._from) << ",dp=" << ntohs(h._to)
               << ",len=" << ntohs(h._length) << ",chk=" << hex << ntohs(h._checksum) << dec << "}";
            return db;
        }

    protected:
        Port _from;
        Port _to;
        unsigned short _length;   // Length of datagram (header + data) in bytes
        unsigned short _checksum; // Pseudo header checksum (see RFC)
    } __attribute__((packed));


    static const unsigned int MTU = 65536 - sizeof(Header) - sizeof(IP::Header);

    typedef unsigned char Data[MTU];


    class Message: public Header
    {
    public:
        Message() {}
        Message(const Port & from, const Port & to, unsigned int size): Header(from, to, size) {}

        Header * header() { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        void sum(const IP::Address & from, const IP::Address & to, const void * data);
        bool check() { return Traits<UDP>::checksum ? (IP::checksum(this, length()) != 0xffff) : true; }

        friend Debug & operator<<(Debug & db, const Message & m) {
            db << "{head=" << reinterpret_cast<const Header &>(m) << ",data=" << m._data << "}";
            return db;
        }

    private:
        Data _data;
    } __attribute__((packed));

public:
    UDP() {
        db<UDP>(TRC) << "UDP::DUP()" << endl;
        IP::attach(this, IP::UDP);
    }
    ~UDP() {
        db<UDP>(TRC) << "UDP::~DUP()" << endl;
        IP::detach(this, IP::UDP);
    }

    int send(const Port & from, const Address & to, const void * data, unsigned int size);
    int receive(NIC::Buffer * buf, void * data, unsigned int size);

private:
    void update(IP::Observed * ip, int port, NIC::Buffer * buf);

    static unsigned short checksum(const Pseudo_Header * pseudo, const Header * header, const void * data, unsigned int size);

private:
    static List _received;
};

__END_SYS

#endif
