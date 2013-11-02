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
        unsigned char * data() { return _data; }

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
        typedef Simple_Ordered_List<Buffer, unsigned long> List;
        typedef List::Element Element;

    public:
        Buffer() {}

        Frame * frame() { return this; }

        unsigned short size() const { return _prot; }
        void size(unsigned short s) { _prot = s; }

        void * back() const { return reinterpret_cast<void *>(_crc); }
        void back(void * b) { _crc = reinterpret_cast<CRC>(b); }

        Element * link() { return reinterpret_cast<Element *>(this); }
        void link(const Element & e) { *reinterpret_cast<Element *>(this) = e; }

        Buffer * next() { return link()->object(); }
        void next(const Buffer * b) { link(Element(b, link()->rank())); }
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

protected:
    Ethernet() {}

public:
    static const unsigned int mtu() { return MTU; }
    static const Address broadcast() { return Address(Address::BROADCAST); }

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
