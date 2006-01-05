// EPOS-- Network Interface Mediator Common Package

#ifndef __nic_h
#define __nic_h

#include <system/config.h>
#include <utility/debug.h>
#include <utility/string.h>

__BEGIN_SYS

class NIC_Common
{
protected:
    NIC_Common() {}

public:
    class Address
    {
    public:
	Address() {}
	Address(const unsigned char * a) {
	    for(int i = 0; i < 8; i++)
		b[i] = a[i];
	}
	Address(unsigned char a0, unsigned char a1,
		unsigned char a2, unsigned char a3,
		unsigned char a4, unsigned char a5,
		unsigned char a6 = 0, unsigned char a7 = 0) {
	    b[0] = a0; b[1] = a1; b[2] = a2; b[3] = a3; 
	    b[4] = a4; b[5] = a5; b[6] = a6; b[7] = a7;
	}
	Address(unsigned short a0, unsigned short a1,
		unsigned short a2, unsigned short a3 = 0) {
	    w[0] = a0;
	    w[1] = a1;
	    w[2] = a2;
	    w[3] = a3; 
	}
	Address(unsigned long a0, unsigned long a1 = 0) {
	    d[0] = a0; d[1] = a1;
	}
	Address(unsigned long long a) : q(a) {}

	operator bool() const { return q; }

	friend Debug & operator << (Debug & db, const Address & a) {
	    db << "{";
	    for(int i = 0; i < 8; i++) {
		db << a.b[i];
		if(i < 7)
		    db << ":";
	    }
	    db << "}";
	    return db;
	}

    public:
    	union {
	    unsigned char b[8];
	    unsigned short w[4];
	    unsigned long d[2];
	    unsigned long long q;
	};
    };
    static const unsigned long long BROADCAST = ~0;

    typedef unsigned short Protocol;

    struct Statistics {
	Statistics() : rx_packets(0), tx_packets(0), 
		       rx_bytes(0), tx_bytes(0) {}

	unsigned int rx_packets;
	unsigned int tx_packets;
	unsigned int rx_bytes;
	unsigned int tx_bytes;
    };

    // Polymorphic (or not) NIC wrapper
    class NIC_Base
    {
    public:
	NIC_Base(unsigned int unit = 0) {}
	virtual ~NIC_Base() {}
    
	virtual int send(const Address & dst, const Protocol & prot, 
			 const void * data, unsigned int size) = 0; 
	virtual int receive(Address * src, Protocol * prot,
			    void * data, unsigned int size) = 0;
    
	virtual void reset() = 0;
    
	virtual unsigned int mtu() = 0;

	virtual const Address & address() = 0;

	virtual const Statistics & statistics() = 0;
    };
    template<typename NIC, bool polymorphic>
    class NIC_Wrapper: public NIC_Base, private NIC
    {
    public:
	NIC_Wrapper(unsigned int unit = 0): NIC(unit) {}
	virtual ~NIC_Wrapper() {}

	virtual int send(const Address & dst, const Protocol & prot, 
			 const void * data, unsigned int size) {
	    return NIC::send(dst, prot, data, size); 
	}
	virtual int receive(Address * src, Protocol * prot,
			    void * data, unsigned int size) {
	    return NIC::receive(src, prot, data, size); 
	}
    
	virtual void reset() { NIC::reset(); }
    
	virtual unsigned int mtu() { return NIC::mtu(); }

	virtual const Address & address() { return NIC::address(); }

	virtual const Statistics & statistics() { return NIC::statistics(); }
    };
    template<typename NIC>
    class NIC_Wrapper<NIC, false>: public NIC
    {
    public:
	NIC_Wrapper(unsigned int unit = 0): NIC(unit) {}
    };

    template<typename NICS>
    class Meta_NIC
    {
    private:
	static const bool polymorphic = NICS::Polymorphic;

    public:
	typedef 	
	typename IF<polymorphic,
		    NIC_Base, 
		    typename NICS::template Get<0>::Result>::Result Base;

	template<int Index>
	struct Get
	{ 
	    typedef
	    NIC_Wrapper<typename NICS::template Get<Index>::Result,
			polymorphic> Result;
	};
    };
};

class Ethernet_NIC: public NIC_Common
{
protected:
    Ethernet_NIC() {}

public:
    static const unsigned int MTU = 1500;
    static const unsigned int HEADER_SIZE = 14;

    // The Ethernet Frame (RFC 894)
    class Frame {
    public:
	Frame(const Address & dst, const Address & src,
	      const Protocol & prot) {
	    header(dst.b, src.b, prot);
	}
	Frame(const Address & dst, const Address & src,
	      const Protocol & prot, const void * data, unsigned int size) {
	    header(dst.b, src.b, prot);
	    memcpy(_data, data, size);
	}
	
	friend Debug & operator << (Debug & db, const Frame & f) {
	    db << "{" << Address(f._dst)
	       << "," << Address(f._src)
	       << "," << f._prot
	       << "," << f._data << "}";
	    return db;
	}
	
    private:
	void header(const unsigned char * dst, const unsigned char * src,
		    const Protocol & prot) {
	    for(int i = 0; i < 6; i++)
		_src[i] = src[i];
	    for(int i = 0; i < 6; i++)
		_dst[i] = dst[i];
	    _prot = prot;
	}

    public:
	unsigned char _src[6];
	unsigned char _dst[6];
	unsigned short _prot;
	char _data[MTU];
	unsigned int _crc;
    };

    // Some valid Ethernet frame types
    enum {
	IP   = 0x0800,
	ARP  = 0x0806,
	RARP = 0x8035
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
};

__END_SYS

#ifdef __NIC_H
#include __NIC_H
#endif

#endif
