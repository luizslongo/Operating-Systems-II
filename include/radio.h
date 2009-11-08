// EPOS-- Low-power Radio Mediator Common Package

#include <nic.h>

#ifndef __radio_h
#define __radio_h

__BEGIN_SYS

class Low_Power_Radio: private NIC_Common
{
protected:
    Low_Power_Radio() {}

public:
    static const unsigned int MTU = 24;  
    static const unsigned int HEADER_SIZE = 6;
    static const unsigned int TRAILER_SIZE = 2;

    typedef unsigned char Address;
    typedef unsigned char Protocol;

    class Frame {
    public:
	Frame() {}

	Frame(const Address & dst, const Address & src,
	      const Protocol & prot, const unsigned char & size) {
	    header(dst, src, prot, size);
	}

	Frame(const Address & dst, const Address & src,
	      const Protocol & prot, const void * data,
	      const unsigned char & size)
	{
	    header(dst, src, prot, size);
	    memcpy(_data, data, size);
	    _crc = CRC::crc16((char*)this, sizeof(Frame)-2);
	}
	
	Frame(const Address & dst, const Address & src,
	      const Protocol & prot, const void * data, 
	      const unsigned char & size, const char & tx_pow,
	      const char & rss)
	{
	    header(dst, src, prot, size);
	    memcpy(_data, data, size);
	    _tx_pow = tx_pow;
	    _rss = rss;
	    _crc = CRC::crc16((char*)this, sizeof(Frame)-2);
	}

	friend Debug & operator << (Debug & db, const Frame & f) {
	    db << "{" << Address(f._dst)
	       << "," << Address(f._src)
	       << "," << f._prot
	       << "," << f._len
	       << "," << f._data << "}";
	    return db;
	}
	
    private:
	void header(const unsigned char & dst, const unsigned char & src,
		    const unsigned char & prot, const unsigned char & len)
	{
	    _src = src;
	    _dst = dst;
	    _prot = prot;
	    _len = len;
	}

    public:
	unsigned char _src;
	unsigned char _dst;
	unsigned char _prot;
	unsigned char _len;
	unsigned char _tx_pow;
	unsigned char _rss;
	char _data[MTU];
	unsigned short _crc;
    };

    // Frame types
    enum {
	EPOS_LP = 0x01,
    };

    struct Statistics: public NIC_Common::Statistics {
	Statistics() : dropped_packets(0) {}

	friend Debug & operator << (Debug & db, const Statistics & s) {
	    db << "{rxp=" << s.rx_packets
	       << ",rxb=" <<  s.rx_bytes
	       << ",txp=" <<  s.tx_packets
	       << ",txb=" <<  s.tx_bytes
	       << ",drop=" <<  s.dropped_packets
	       << "}";
	    return db;
	}
	
	unsigned int dropped_packets;
    };

};

__END_SYS

#endif
