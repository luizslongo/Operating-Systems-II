// EPOS-- ATMega16 NIC Mediator Declarations

#ifndef __atmega16_nic_h
#define __atmega16_nic_h

#include "atmega16.h"

__BEGIN_SYS

class ATMega16_NIC
{
private:
    typedef Traits<ATMega16_NIC> Traits;
    static const int TYPE = Type<ATMega16_NIC>::TYPE;

    typedef CPU::Phy_Addr Phy_Addr;
    typedef MMU::DMA_Buffer DMA_Buffer;

public:
    // The actual NIC
public:
    //Dummy stuff
    class Device {
    public:
        typedef int Address;
	typedef int Protocol;
	typedef int Statistics;

        int send(const Address & dst, const Protocol & prot, 
		 const void * data, unsigned int size) { return -1; }
	int receive(Address * src, Protocol * prot,
		    void * data, unsigned int size) { return -1; }
	const Statistics & statistics() { return -1; }
	void reset() { }
    };
    static const unsigned int MTU = 0;



    // Transmit and Receive Ring (with buffers) sizes
    static const unsigned int UNITS = 1;
    static const unsigned int DMA_BUFFER_SIZE = 0;

public:

    //Dummy types
    typedef int Address;
    typedef int Protocol;
    typedef int Statistics;


    ATMega16_NIC(unsigned int unit = 0) : _unit(unit) {
	db<ATMega16_NIC>(TRC) << "ATMega16_NIC(unit=" << unit << ")\n";
	_dev = Machine::seize<Device>(TYPE, _unit);
    }

    ~ATMega16_NIC() {
	db<ATMega16_NIC>(TRC) << "~ATMega16_NIC()\n";
	Machine::release(TYPE, _unit);
	_dev = 0;
    }

    int send(const Address & dst, const Protocol & prot, 
	     const void * data, unsigned int size) {
	if(size > MTU)
	    db<ATMega16_NIC>(WRN) << "ATMega16_NIC::send: frame size exceeds MTU!\n";

	return _dev->send(dst, prot, data, size);
    }

    int receive(Address * src, Protocol * prot,
		void * data, unsigned int size) {
	if(size > MTU)
	    db<ATMega16_NIC>(WRN) << "ATMega16_NIC::receive: frame size exceeds MTU!\n";

	return _dev->receive(src, prot, data, size);
    }

    const Statistics & statistics() { return _dev->statistics(); }

    void reset() { _dev->reset(); }

    static int init(System_Info * si);

private:
    unsigned int _unit;
    Device * _dev;
};

typedef ATMega16_NIC NIC;

__END_SYS

#endif
