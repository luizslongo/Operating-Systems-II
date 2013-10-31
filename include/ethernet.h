// EPOS Ethernet Mediator Common Package

#include <nic.h>

#ifndef __ethernet_h
#define __ethernet_h

#include <utility/string.h>

__BEGIN_SYS

class Ethernet: private NIC_Common
{
protected:
    static const unsigned int MTU = 1500;
    static const unsigned int HEADER_SIZE = 14;


public:
    typedef NIC_Common::Address<6> Address;
    typedef unsigned char Data[MTU];
    typedef NIC_Common::CRC32 CRC;

    typedef unsigned short Protocol;
    enum
    {
        IP     = 0x0800,
        ARP    = 0x0806,
        RARP   = 0x8035,
        ELP    = 0x8888,
        PTP    = 0x88F7
    };


    class Buffer;

    // The Ethernet Header (RFC 894)
    class Header
    {
        friend class Buffer;

    public:
        Header() {}
        Header(const Address & src, const Address & dst, const Protocol & prot)
        : _dst(dst), _src(src), _prot(htons(prot)) {}

        friend Debug & operator<<(Debug & db, const Header & h) {
            db << "{" << h._dst << "," << h._src << "," << h.prot() << "}";
            return db;
        }

        const Address & src() const { return _src; }
        const Address & dst() const { return _dst; }

        Protocol prot() const { return ntohs(_prot); }

    private:
        Address _dst;
        Address _src;
        Protocol _prot;
    } __attribute__((packed, may_alias));


    // The Ethernet Frame (RFC 894)
    class Frame
    {
    public:
        Frame() {}
        Frame(const Address & src, const Address & dst, const Protocol & prot)
        : _header(src, dst, prot) {}

        Frame(const Address & src, const Address & dst, const Protocol & prot, const void * data, unsigned int size)
        : _header(src, dst, prot) {
            memcpy(_data, data, size);
        }
        
        friend Debug & operator<<(Debug & db, const Frame & f) {
            db << "{" << f._header.dst() << "," << f._header.src() << "," << f._header.prot() << "," << f._data << "}";
            return db;
        }
        
        Header * header() { return &_header; }
        unsigned char * data() { return _data; }

    protected:
        Header _header;
        Data _data;
        CRC _crc;
    } __attribute__((packed, may_alias));
    typedef Frame PDU;


    // Buffers used to hold frames across a zero-copy network stack
    // Everything but the data will be overwritten with buffer metainfo
    // The sizeof(List::Element) must ALWAYS be less than 12!
    class Buffer: private Frame
    {
    public:
        Buffer() {}

        Frame * frame() { return this; }

        unsigned short size() const { return _header._prot; }
        void size(unsigned short s) { _header._prot = s; }

        void * back() const { return reinterpret_cast<void *>(_crc); }
        void back(void * b) { _crc = reinterpret_cast<CRC>(b); }

        template<typename T>
        T * link() { return reinterpret_cast<T *>(this); }
        template<typename T>
        void link(const T & e) { *reinterpret_cast<T *>(this) = e; }
    };


    // Observers of a protocol get a also a pointer to the received buffer
    typedef Data_Observer<Buffer> Observer;
    typedef Data_Observed<Buffer> Observed;


    // Meaningful statistics for Ethernet
    struct Statistics: public NIC_Common::Statistics
    {
        Statistics() : rx_overruns(0), tx_overruns(0), frame_errors(0), carrier_errors(0), collisions(0) {}

        friend Debug & operator<<(Debug & db, const Statistics & s) {
            db << "{rxp=" << s.rx_packets
               << ",rxb=" <<  s.rx_bytes
               << ",rxorun=" <<  s.rx_overruns
               << ",txp=" <<  s.tx_packets
               << ",txb=" <<  s.tx_bytes
               << ",txorun=" <<  s.tx_overruns
               << ",frm=" <<  s.frame_errors
               << ",car=" <<  s.carrier_errors
               << ",col=" <<  s.collisions
               << "}";
            return db;
        }
        
        unsigned int rx_overruns;
        unsigned int tx_overruns;
        unsigned int frame_errors;
        unsigned int carrier_errors;
        unsigned int collisions;
    };

    static const Address BROADCAST;

protected:
    Ethernet() {}

public:
    void attach(Observer * obs, const Protocol & prot) {
        _observed.attach(obs, prot);
    }

    void detach(Observer * obs, const Protocol & prot) {
        _observed.detach(obs, prot);
    }

    void notify(const Protocol & prot, Buffer * buf) {
        _observed.notify(prot, buf);
    }

private:
    static Observed _observed; // Shared by all units of all models
};

__END_SYS

#endif

