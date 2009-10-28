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

// 	MAC_Address dst = arp(to);
// 	if(!dst)
// 	    return -1;

 	MAC_Address dst = NIC::BROADCAST;

	return _nic.send(dst, prot, data, size);
    }

    int receive(Address * from, void * data, unsigned int size,
		Protocol * prot) {
	if(size > _nic.mtu())
	    // Defragmentation will take place here
	    db<Network>(WRN) << "Network::receive: frame size exceeds MTU!\n";

	MAC_Address src;
	int stat = _nic.receive(&src, prot, data, size);

// 	if(stat > 0)
// 	    *from = rarp(src);

	*from = BROADCAST;
	
	return stat;
    }

    int receive(Address * from, void * data, unsigned int size) {
	Protocol p;
	return receive(from, data, size, &p);
    }

    void reset() { _nic.reset(); }

    const Statistics & statistics() { return _nic.statistics(); }

private:
    NIC _nic;
};

__END_SYS

#endif
