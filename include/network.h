// EPOS-- Network Abstraction Declarations

#ifndef __network_h
#define __network_h

#include <machine.h>
#include <nic.h>
#include <arp.h>

__BEGIN_SYS

class Network: public NIC::Observer
{
public:
    // Network logical address
    class Address: public NIC_Common::Address<4> {
    public:
	Address() {}
	Address(int a) { *reinterpret_cast<int *>(this) = a; }
	Address(unsigned char a0, unsigned char a1 = 0,
		unsigned char a2 = 0, unsigned char a3 = 0)
	    : NIC_Common::Address<4>(a0, a1, a2, a3) {}

	operator int() { return *reinterpret_cast<int *>(this); }
	operator int() const { return *reinterpret_cast<const int *>(this); }
    };
    static const Address BROADCAST;

    // Network phisical address
    typedef NIC::Address MAC_Address;

    // Network protocol numbers
    typedef unsigned short Protocol;
    enum {
	PROT_ELP  = NIC::ELP,
	PROT_IP   = NIC::IP,
	PROT_ARP  = NIC::ARP,
	PROT_RARP = NIC::RARP
    };

    // Network statistics
    typedef NIC::Statistics Statistics;

    // ARP
    typedef ARP<NIC, Network> ARP;
    
public:
    Network() {
	db<Network>(TRC) << "Network(unit=0)\n";

	_arpt.update(BROADCAST, NIC::BROADCAST);
	_nic.attach(this, PROT_ARP);
	_nic.attach(this, PROT_RARP);
	_address = rarp(_nic.address());
    }

    template<unsigned int UNIT>
    Network(unsigned int unit) : _nic(UNIT) {
	db<Network>(TRC) << "Network(unit=" << unit << ")\n";
    }

    ~Network() {
	db<Network>(TRC) << "~Network()\n";
    }

    int send(const Address & to, const void * data, unsigned int size);

    int receive(Address * from, void * data, unsigned int size);

    MAC_Address arp(const Address & addr);
    Address rarp(const MAC_Address & addr);

    void update(NIC::Observed * o, int p);

    void reset() { _nic.reset(); }

    const Address & address() { return _address; }

    const Statistics & statistics() { return _nic.statistics(); }

private:
    NIC _nic;
    Address _address;

    static ARP::Table _arpt;
};

__END_SYS

#endif
