// EPOS-- Ethernet Mediator Common Package

#include <nic.h>

#ifndef __ethernet_h
#define __ethernet_h

__BEGIN_SYS

class Ethernet: private NIC_Common
{
protected:
    Ethernet() {}

public:
    static const unsigned int MTU = 1500;
    static const unsigned int HEADER_SIZE = 14;

    typedef NIC_Common::Address<6> Address;
    typedef NIC_Common::Protocol Protocol;
    typedef NIC_Common::Observer Observer;
    typedef NIC_Common::Observed Observed;
    typedef char PDU[MTU];
    typedef unsigned int CRC;

    // Some valid Ethernet frame types
    enum {
	IP   = 0x0800,
	ARP  = 0x0806,
	RARP = 0x8035,
	ELP  = Traits<Network>::ELP
    };

    // The Ethernet Frame (RFC 894)
    class Frame {
    public:
	Frame(const Address & src, const Address & dst,
	      const Protocol & prot): _src(src), _dst(dst), _prot(prot) { }

	Frame(const Address & src, const Address & dst,
	      const Protocol & prot, const void * data, unsigned int size)
	    : _src(src), _dst(dst), _prot(prot)
	{
	    memcpy(_data, data, size);
	}
	
	friend Debug & operator << (Debug & db, const Frame & f) {
	    db << "{" << Address(f._dst)
	       << "," << Address(f._src)
	       << "," << f._prot
	       << "," << f._data << "}";
	    return db;
	}
	
    public:
	Address _src;
	Address _dst;
	Protocol _prot;
	PDU _data;
	CRC _crc;
    };

    // Meaningful statistics for Ethernet
    struct Statistics: public NIC_Common::Statistics {
	Statistics() : rx_overruns(0), tx_overruns(0), frame_errors(0),
		       carrier_errors(0), collisions(0) {}

	friend Debug & operator << (Debug & db, const Statistics & s) {
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

public:
    void attach(Observer * obs, const Protocol & prot) {
	_observed.attach(obs, prot);
    }

    void detach(Observer * obs, const Protocol & prot) {
	_observed.detach(obs, prot);
    }

    void notify(const Protocol & prot) {
	_observed.notify(prot);
    }


public:
    static const Address BROADCAST;

private:
    static Observed _observed;
};

__END_SYS

#endif
