// EPOS Ethernet Mediator Common Package

#include <nic.h>

#ifndef __ethernet_h
#define __ethernet_h

#include <cpu.h>
#include <utility/list.h>
#include <utility/observer.h>

__BEGIN_SYS

class Ethernet: private NIC_Common
{
protected:
    static const unsigned int MTU = 1500;
    static const unsigned int HEADER_SIZE = 14;


public:
    class Address: public NIC_Common::Address<6>
    {
    private:
        typedef NIC_Common::Address<6> Base;

    public:
        enum Broadcast { BROADCAST };

    public:
        Address() {}
        Address(unsigned char a0, unsigned char a1 = 0, unsigned char a2 = 0, unsigned char a3 = 0, unsigned char a4 = 0, unsigned char a5 = 0): Base(a0, a1, a2, a3, a4, a5) {}
        Address(const char * s): Base(s, ':') {}
        Address(const Broadcast & b): Base(0xff, 0xff, 0xff, 0xff, 0xff, 0xff) {}
    } __attribute__((packed, may_alias));

    typedef unsigned short Protocol;
    enum
    {
        IP     = 0x0800,
        ARP    = 0x0806,
        RARP   = 0x8035,
        ELP    = 0x8888,
        PTP    = 0x88F7
    };

    typedef unsigned char Data[MTU];
    typedef NIC_Common::CRC32 CRC;


    // The Ethernet Header (RFC 894)
    class Header
    {
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

    protected:
        Address _dst;
        Address _src;
        Protocol _prot;
    } __attribute__((packed, may_alias));


    // The Ethernet Frame (RFC 894)
    class Frame: public Header
    {
    public:
        Frame() {}
        Frame(const Address & src, const Address & dst, const Protocol & prot) : Header(src, dst, prot) {}
        Frame(const Address & src, const Address & dst, const Protocol & prot, const void * data, unsigned int size): Header(src, dst, prot) {
            memcpy(_data, data, size);
        }
        
        Header * header() { return this; }

        template<typename T>
        T * data() { return reinterpret_cast<T *>(&_data); }

        friend Debug & operator<<(Debug & db, const Frame & f) {
            db << "{" << f.dst() << "," << f.src() << "," << f.prot() << "," << f._data << "}";
            return db;
        }
        
    protected:
        Data _data;
        CRC _crc;
    } __attribute__((packed, may_alias));

    typedef Frame PDU;


    // Buffers used to hold frames across a zero-copy network stack
    // Everything but the data is meant to be overwritten with meta info
    class Buffer: private Frame
    {
    public:
        typedef Simple_List<Buffer> List;
        typedef List::Element Element;

    public:
        Buffer(void * b): _lock(false), _size(sizeof(Frame)), _back(b), _link(this) {}

        Frame * frame() { return this; }

//        unsigned short size() const { return _prot; }
//        void size(unsigned short s) { _prot = s; }
//
//        void * back() const { return reinterpret_cast<void *>(_crc); }
//        void back(void * b) { _crc = reinterpret_cast<CRC>(b); }
//
//        NIC * nic() const { return _nic; }
//        void nic(NIC * n) { _nic = n; }
//
//        bool lock() { return !CPU::tsl(_lock); }
//        void unlock() { _lock = 0; }
//
//        Element * link() { return reinterpret_cast<Element *>(this); }
//        void link(const Element & e) { *reinterpret_cast<Element *>(this) = e; }
//
//        Buffer * next() { return link()->object(); }
//        void next(const Buffer * b) { link(Element(b, link()->rank())); }

        NIC * nic() const { return _nic; }
        void nic(NIC * n) { _nic = n; }

        bool lock() { return !CPU::tsl(_lock); }
        void unlock() { _lock = 0; }

        unsigned int size() const { return _size; }
        void size(unsigned int s) { _size = s; }

        template<typename T>
        T * back() const { return reinterpret_cast<T *>(_back); }
//        template<typename T>
//        void back(T * b) { _back = b; }

        Element & link() { return _link; }
//        void link(const Element & e) { _link1 = e; }
//
//        Buffer * next() { return _link2.object(); }
//        void next(const Buffer * b) { _link2 = Element(b, _link2.rank()); }

        friend Debug & operator<<(Debug & db, const Buffer & b) {
            db << "{nc=" << b._nic << ",lk=" << b._lock << ",sz=" << b._size << ",bl=" << b._back << "}";
            return db;
        }

    private:
        NIC * _nic;
        volatile bool _lock;
        unsigned int _size;
        void * _back;
        Element _link;
    };


    // Observers of a protocol get a also a pointer to the received buffer
    typedef Data_Observer<Buffer> Observer;
    typedef Data_Observed<Buffer> Observed;


    // Meaningful statistics for Ethernet
    struct Statistics: public NIC_Common::Statistics
    {
        Statistics(): rx_overruns(0), tx_overruns(0), frame_errors(0), carrier_errors(0), collisions(0) {}

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

protected:
    Ethernet() {}

public:
    static const unsigned int mtu() { return MTU; }
    static const Address broadcast() { return Address(Address::BROADCAST); }

    void attach(Observer * obs, const Protocol & prot) { _observed.attach(obs, prot); }
    void detach(Observer * obs, const Protocol & prot) { _observed.detach(obs, prot); }
    void notify(const Protocol & prot, Buffer * buf) { _observed.notify(prot, buf); }

private:
    static Observed _observed; // Shared by all units of all models
};

__END_SYS

#endif
