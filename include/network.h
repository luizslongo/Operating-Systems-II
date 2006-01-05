// EPOS-- Network Abstraction Declarations

#ifndef __network_h
#define __network_h

#include <machine.h>
#include <nic.h>

__BEGIN_SYS

class Network
{
public:
    // Network logical address
    typedef unsigned int Address;
    static const unsigned int BROADCAST = ~0;

    // Network protocol number
    typedef NIC::Protocol Protocol;
    enum {
	ELP  = Traits<Network>::EPOS_LIGHT_PROTOCOL,
	IP   = 0x0800,
	ARP  = 0x0806,
	RARP = 0x8035
    };

    // Network statistics
    typedef NIC::Statistics Statistics;

private:
    // Network phisical address
    typedef NIC::Address MAC_Address;

    // Address Resolution Protocol 
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
    Network() {
	db<Network>(TRC) << "Network(unit=0)\n";
    }

    template<unsigned int UNIT>
    Network(unsigned int unit) : _nic(UNIT) {
	db<Network>(TRC) << "Network(unit=" << unit << ")\n";
    }

    ~Network() {
	db<Network>(TRC) << "~Network()\n";
    }

    int send(const Address & to, const void * data, unsigned int size, 
	     const Protocol & prot = ELP) {
	if(size > _nic.mtu())
	    // Fragmentation will take place here
	    db<Network>(WRN) << "Network::send: frame size exceeds MTU!\n";

	MAC_Address dst = _arp_tab.arp(to);
	if(!dst)
	    return -1;

	return _nic.send(dst, prot, data, size);
    }

    int receive(Address * from, void * data, unsigned int size,
		Protocol * prot) {
	if(size > _nic.mtu())
	    // Defragmentation will take place here
	    db<Network>(WRN) << "Network::receive: frame size exceeds MTU!\n";

	MAC_Address src;
	int stat = _nic.receive(&src, prot, data, size);

	if(stat > 0)
	    *from = _arp_tab.rarp(src);

	return stat;
    }

    int receive(Address * from, void * data, unsigned int size) {
	Protocol p;
	return receive(from, data, size, &p);
    }

    void reset() { _nic.reset(); }

    const Statistics & statistics() { return _nic.statistics(); }

    static int init(System_Info * si) { return 0; }

private:
    NIC _nic;

    static ARP_Table _arp_tab;
};

__END_SYS

#endif
