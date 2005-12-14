// EPOS-- ATMega128 NIC Mediator Declarations

#ifndef __atmega128_nic_h
#define __atmega128_nic_h

#include <machine.h>
#include "../avr_common/cc1000.h"

__BEGIN_SYS

class ATMega128_NIC: public NIC_Common, private CC1000
{
private:
    typedef CPU::Phy_Addr Phy_Addr;

public:
    typedef Radio Device;

    static const unsigned int UNITS = 1;
    static const unsigned int MTU = 1500;

public:
    ATMega128_NIC(unsigned int unit = 0) : _unit(unit) {
	db<ATMega128_NIC>(TRC) << "ATMega128_NIC(unit=" << unit << ")\n";
	_dev = Machine::seize<Device>(NIC_ID, _unit);
    }

    ~ATMega128_NIC() {
	db<ATMega128_NIC>(TRC) << "~ATMega128_NIC()\n";
	Machine::release(NIC_ID, _unit);
	_dev = 0;
    }

    int send(const Address & dst, const Protocol & prot, 
	     const void * data, unsigned int size) {
	if(size > MTU)
	    db<ATMega128_NIC>(WRN) << "ATMega128_NIC::send: frame size exceeds MTU!\n";

	return _dev->send(dst, prot, data, size);
    }

    int receive(Address * src, Protocol * prot,
		void * data, unsigned int size) {
	if(size > MTU)
	    db<ATMega128_NIC>(WRN) << "ATMega128_NIC::receive: frame size exceeds MTU!\n";

	return _dev->receive(src, prot, data, size);
    }

    const Statistics & statistics() { return _dev->statistics(); }

    void reset() { _dev->reset(); }

    static int init(System_Info * si) { return 0; }

private:
    unsigned int _unit;
    Device * _dev;
};

typedef ATMega128_NIC NIC;

__END_SYS

#endif
