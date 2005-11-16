// EPOS-- ATMega128 NIC Mediator Declarations

#ifndef __atmega128_nic_h
#define __atmega128_nic_h

#include "atmega128.h"
#include "../common/cc1000.h"

__BEGIN_SYS

class ATMega128_NIC: public CC1000
{
private:
    typedef Traits<ATMega128_NIC> Traits;
    static const int TYPE = Type<ATMega128_NIC>::TYPE;

    typedef CPU::Phy_Addr Phy_Addr;
    typedef MMU::DMA_Buffer DMA_Buffer;

public:
    // The actual NIC
public:
    typedef CC1000 Device;

    // Transmit and Receive Ring (with buffers) sizes
    static const unsigned int UNITS = 1;
    static const unsigned int DMA_BUFFER_SIZE = 0;

public:
    ATMega128_NIC(unsigned int unit = 0) : _unit(unit) {
	db<ATMega128_NIC>(TRC) << "ATMega128_NIC(unit=" << unit << ")\n";
	_dev = Machine::seize<Device>(TYPE, _unit);
    }

    ~ATMega128_NIC() {
	db<ATMega128_NIC>(TRC) << "~ATMega128_NIC()\n";
	Machine::release(TYPE, _unit);
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

    static int init(System_Info * si);

private:
    unsigned int _unit;
    Device * _dev;
};

typedef ATMega128_NIC NIC;

__END_SYS

#endif
