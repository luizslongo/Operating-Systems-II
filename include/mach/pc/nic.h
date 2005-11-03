// EPOS-- PC NIC Mediator Declarations

#ifndef __pc_nic_h
#define __pc_nic_h

#include "pc.h"
#include "pcnet32.h"

__BEGIN_SYS

class PC_NIC: public Ethernet_NIC
{
private:
    typedef Traits<PC_NIC> Traits;
    static const int TYPE = Type<PC_NIC>::TYPE;

    typedef CPU::IO_Port IO_Port;
    typedef CPU::IO_Irq IO_Irq;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef MMU::DMA_Buffer DMA_Buffer;

public:
    // The actual NIC
    typedef PCNet32 Device;

    // PCI ID
    static const unsigned int PCI_VENDOR_ID = Device::PCI_VENDOR_ID;
    static const unsigned int PCI_DEVICE_ID = Device::PCI_DEVICE_ID;
    static const unsigned int PCI_REG_IO = Device::PCI_REG_IO;
    static const unsigned int PCI_REG_MEM = Device::PCI_REG_MEM;

    // Transmit and Receive Ring (with buffers) sizes
    static const unsigned int UNITS = Device::UNITS;
    static const unsigned int DMA_BUFFER_SIZE = Device::DMA_BUFFER_SIZE;

public:
    PC_NIC(unsigned int unit = 0) : _unit(unit) {
	db<PC_NIC>(TRC) << "PC_NIC(unit=" << unit << ")\n";
	_dev = Machine::seize<Device>(TYPE, _unit);
    }

    ~PC_NIC() {
	db<PC_NIC>(TRC) << "~PC_NIC()\n";
	Machine::release(TYPE, _unit);
	_dev = 0;
    }

    int send(const Address & dst, const Protocol & prot, 
	     const void * data, unsigned int size) {
	if(size > MTU)
	    db<PC_NIC>(WRN) << "PC_NIC::send: frame size exceeds MTU!\n";

	return _dev->send(dst, prot, data, size);
    }

    int receive(Address * src, Protocol * prot,
		void * data, unsigned int size) {
	if(size > MTU)
	    db<PC_NIC>(WRN) << "PC_NIC::receive: frame size exceeds MTU!\n";

	return _dev->receive(src, prot, data, size);
    }

    const Statistics & statistics() { return _dev->statistics(); }

    void reset() { _dev->reset(); }

    static int init(System_Info * si);

private:
    unsigned int _unit;
    Device * _dev;
};

typedef PC_NIC NIC;

__END_SYS

#endif
