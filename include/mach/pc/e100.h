// EPOS-- PC Intel PRO/100 (i82559) Ethernet NIC Mediator Declarations

#ifndef __e100_h
#define __e100_h

#include <nic.h>

__BEGIN_SYS

class i82559
{
};

class E100: public Ethernet_NIC, private i82559
{
private:
    // PCI ID
    static const unsigned int PCI_VENDOR_ID = 0x0000;
    static const unsigned int PCI_DEVICE_ID = 0x0000;
    static const unsigned int PCI_REG_IO = 0;
    static const unsigned int PCI_REG_MEM = 0;

    // Transmit and Receive Ring Bbuffer sizes
    static const unsigned int UNITS =
	Traits<PC_NIC>::E100_UNITS;
    static const unsigned int TX_BUFS =
	Traits<PC_NIC>::E100_SEND_BUFFERS;
    static const unsigned int RX_BUFS =
	Traits<PC_NIC>::E100_RECEIVE_BUFFERS;
    static const unsigned int DMA_BUFFER_SIZE = 0; // GCC mess up MMU::align128

    // Share control and interrupt dispatiching info
    struct Device
    {
	E100 * device;
	unsigned int interrupt;
	bool in_use;
    };
	
public:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::IO_Irq IO_Irq;
    typedef MMU::DMA_Buffer DMA_Buffer;

public:
    E100(unsigned int unit = 0);
    ~E100();

    int send(const Address & dst, const Protocol & prot,
 	     const void * data, unsigned int size);
    int receive(Address * src, Protocol * prot,
		void * data, unsigned int size);

    void reset();

    unsigned int mtu() { return MTU; }

    const Address & address() { return _address; }

    const Statistics & statistics() { return _statistics; }

    static int init(unsigned int unit, System_Info * si);

private:
    E100(unsigned int unit, Log_Addr io_mem, IO_Irq irq, DMA_Buffer * dma);

    void handle_int();

    static void int_handler(unsigned int interrupt);

    static E100 * get(unsigned int interrupt) {
	for(unsigned int i = 0; i < UNITS; i++)
	    if(_devices[i].interrupt == interrupt)
		return _devices[i].device;
	return 0;
    };

private:
    unsigned int _unit;

    Address _address;
    Statistics _statistics;

    Log_Addr _io_mem;
    IO_Irq _irq;
    DMA_Buffer * _dma_buf;

    static Device _devices[UNITS];
};

__END_SYS

#endif
