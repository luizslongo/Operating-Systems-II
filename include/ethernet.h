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

    // The Ethernet Header (RFC 894)
    class Header
    {
    public:
        Header(const Address & src, const Address & dst, const Protocol & prot)
        : _dst(dst), _src(src), _prot(prot) {}

        friend Debug & operator<<(Debug & db, const Header & h) {
            db << "{" << h._dst << "," << h._src << "," << h._prot << "}";
            return db;
        }

        const Address & src() const { return _src; }
        const Address & dst() const { return _dst; }
        Protocol prot() const { return _prot; }

    private:
        Address _dst;
        Address _src;
        Protocol _prot;
    } __attribute__((packed, may_alias));

    // The Ethernet Frame (RFC 894)
    class Frame
    {
    public:
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
        
        Header & header() { return _header; }
        unsigned char * data() { return _data; }

        const Address & src() const { return _header.src(); }
        const Address & dst() const { return _header.dst(); }
        Protocol prot() const { return _header.prot(); }

    private:
        Header _header;
        Data _data;
        CRC _crc;
    } __attribute__((packed, may_alias));
    typedef Frame PDU;

    typedef Data_Observer<Frame> Observer;
    typedef Data_Observed<Frame> Observed;

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

    void notify(const Protocol & prot, Frame * pdu) {
        _observed.notify(prot, pdu);
    }

private:
    static Observed _observed; // Shared by all units of all models
};

__END_SYS

#endif

