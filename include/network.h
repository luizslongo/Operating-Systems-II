// EPOS-- Network Abstraction Declarations

#ifndef __network_h
#define __network_h

#include <machine.h>
#include <nic.h>

__BEGIN_SYS

class Network
{
private:
    typedef Traits<Network> Traits;
    static const Type_Id TYPE = Type<Network>::TYPE;

public:
    // A network logical address
    typedef unsigned int Address;
    static const unsigned int BROADCAST = ~0;

    // A network protocol number
    typedef NIC::Protocol Protocol;
    enum {
	ELP  = Traits::EPOS_LIGHT_PROTOCOL,
	IP   = 0x0800,
	ARP  = 0x0806,
	RARP = 0x8035
    };

    // Network statistics
    typedef NIC::Statistics Statistics;

private:
    typedef NIC::Address MAC_Address;

    class ARP_Table {
    public:
	void insert(const Address & log, const MAC_Address & mac) {}
	void remove(const Address & log) {}
	void remove(const MAC_Address & mac) {}
	void remove(const Address & log, const MAC_Address & mac) {}

	const MAC_Address & arp(const Address & log) const { 
	    // to be implemented
	}
	const Address & rarp(const MAC_Address & mac) const {
	    // to be implemented
	}
    };

public:
    Network(unsigned int unit = 0) {
	db<Network>(TRC) << "Network(unit=" << unit << ")\n";
	_unit = unit;
	_dev = Machine::seize<NIC::Device>(Type<NIC>::TYPE, _unit);
    }

    ~Network() {
	db<Network>(TRC) << "~Network()\n";
	Machine::release(Type<NIC>::TYPE, _unit);
	_dev = 0;
    }

    int send(const Address & to, const void * data, unsigned int size, 
	     const Protocol & prot = ELP) {
	if(size > NIC::MTU)
	    // Fragmentation will take place here
	    db<Network>(WRN) << "Network::send: frame size exceeds MTU!\n";

	MAC_Address dst = _arp_tab.arp(to);
	if(!dst)
	    return -1;

	return _dev->send(dst, prot, data, size);
    }

    int receive(Address * from, void * data, unsigned int size,
		Protocol * prot) {
	if(size > NIC::MTU)
	    // Defragmentation will take place here
	    db<Network>(WRN) << "Network::receive: frame size exceeds MTU!\n";

	MAC_Address src;
	int stat = _dev->receive(&src, prot, data, size);

	if(stat > 0)
	    *from = _arp_tab.rarp(src);

	return stat;
    }

    int receive(Address * from, void * data, unsigned int size) {
	Protocol p;
	return receive(from, data, size, &p);
    }

    const Statistics & statistics() { return _dev->statistics(); }

    void reset() { _dev->reset(); }

    static int init(System_Info * si);

private:
    unsigned int _unit;
    NIC::Device * _dev;

    static ARP_Table _arp_tab;
};

__END_SYS

#endif
