#ifndef __udp_h
#define __udp_h

#include "ip.h"
#include <mutex.h>

__BEGIN_SYS 

class UDP_Address {
 public:
    UDP_Address() { }
    UDP_Address(u32 ip, u16 port): _ip(ip), _port(port) { }
    UDP_Address(IP_Address ip, u16 port): _ip(ip), _port(port) { }
    UDP_Address(const char *addr) : _ip(addr) {
        char *sep = strchr(addr,':');
        if (sep) {
            _port = atol(++sep);
        } else {
            _port = 0;
        }
    }

    template < typename T >
    friend T & operator <<(T & out, const UDP_Address & h) {
        out << dec << h.ip() << ":" << (unsigned int)(h.port());
        return out;
    }

    char* to_string(char * dst) {
        char *p = _ip.to_string(dst);
        *p++ = ':';
        p += utoa(_port,p);
        *p = 0;
        return p;
    }

    u16 port() const { return _port; }
    IP_Address ip() const { return _ip; }

    void port(u16 new_port) { _port = new_port; }
    void ip(const IP_Address& ip) { _ip = ip; }
    
    bool operator==(const UDP_Address& other)
    {
        return ip() == other.ip() && port() == other.port();    
    }

 private:
    IP_Address _ip;
    u16 _port;

};

class UDP: public IP::Observer, public Data_Observed < UDP_Address > {
public:
    // UDP ID (IP Frame)
    static const IP::Protocol ID_UDP = 0x11;

    typedef UDP_Address Address;

    class Header;
    class Socket;
    class Channel;

    UDP(IP * ip) : _ip(ip) {
        _ip->attach(this, ID_UDP);
    }

    ~UDP() {
        _ip->detach(this, ID_UDP);
    }

    s32 send(Address local, Address remote, SegmentedBuffer * data);

    // Data_Observer callback
    void update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
                IP::Address dst, void *data, unsigned int size);

    IP * ip() { return _ip; }
    
    static UDP * instance(unsigned int i = 0) {
        if (!_instance[i])
            _instance[i] = new UDP(IP::instance(i));
        return _instance[i];
    }

private:

    struct Pseudo_Header {
        u32 src_ip;
        u32 dst_ip;
        u8 zero;
        u8 protocol;
        u16 length;
        
        Pseudo_Header(u32 src,u32 dst,u16 len) 
        : src_ip(src), dst_ip(dst), zero(0), protocol(ID_UDP),
          length(CPU::htons(len)) {};
    };

    IP *_ip;
    
    static UDP* _instance[Traits<NIC>::NICS::Length];
};

class UDP::Header {
    friend class UDP;
public:
    Header() {}

    Header(u16 src_port = 0, u16 dst_port = 0, u16 data_size = 0)
    : _src_port(CPU::htons(src_port)),
      _dst_port(CPU::htons(dst_port)),
      _length(CPU::htons(sizeof(UDP::Header) + data_size)),
      _checksum(0) { }

    void checksum(IP::Address src,IP::Address dst,SegmentedBuffer * sb);

    u16 dst_port() const { return CPU::ntohs(_dst_port); }
    u16 src_port() const { return CPU::ntohs(_src_port); }

    friend Debug & operator <<(Debug & db, const Header & h) {
        db << "{sprt=" << CPU::ntohs(h._src_port)
            << ",dprt=" << CPU::ntohs(h._dst_port)
            << ",len=" << CPU::ntohs(h._length)
            << ",chk=" << (void *)h._checksum << "}";
        return db;
    }

private:
    u16 _src_port;  // Source UDP port
    u16 _dst_port;  // Destination UDP port
    u16 _length;    // Length of datagram (header + data) in bytes
    volatile u16 _checksum;  // Pseudo header checksum (see RFC)
};

class UDP::Socket: public Data_Observer <UDP_Address> {
    friend class UDP;
public:
    Socket(Address local, Address remote, UDP * udp = 0);

    ~Socket();

    s32 send(const char *data, u16 size) const {
        SegmentedBuffer sb(data, size);
        return send(&sb);
    }
    s32 send(SegmentedBuffer * data) const {
        return _udp->send(_local, _remote, data);
    }

    void local(const Address & local) { _local = local; }
    void remote(const Address & party) { _remote = party; }

    const Address & remote() const { return _remote; }

    void update(Observed * o, long c, UDP_Address src, UDP_Address dst,
                void * data, unsigned int size);

    // every Socket should implement one
    virtual void received(const Address & src,
                          const char *data, unsigned int size) {};
protected:
    Address _local;
    Address _remote;

    UDP *_udp;
};


class UDP::Channel : public Socket
{
public:
    int receive(Address * from,char * buf,unsigned int size);

protected:
    virtual void received(const Address & src,
                          const char *data, unsigned int size);

    Address * _buffer_src;
    Mutex     _buffer_wait;
    unsigned int _buffer_size;
    char       * _buffer_data;
};



__END_SYS
#endif
