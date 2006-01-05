// EPOS-- PC Intel PRO/100 (i82559) Ethernet NIC Mediator Implementation

#include <system/kmalloc.h>
#include <mach/pc/pc.h>
#include <mach/pc/e100.h>

__BEGIN_SYS

// Class attributes
E100::Device E100::_devices[UNITS];

// Class Methods
void E100::int_handler(unsigned int interrupt)
{
    E100 * dev = get(interrupt);

    db<PC_NIC>(TRC) << "E100::int_handler(int=" << interrupt
		    << ",dev=" << dev << ")\n";
    if(!dev)
	db<PC>(WRN) << "E100::int_handler: handler not found\n";
    else 
	dev->handle_int();
}

// Methods
E100::E100(unsigned int unit)
{
    db<E100>(TRC) << "E100(unit=" << unit << ")\n";

    // Share control
    if(unit >= UNITS) {
	db<E100>(WRN) << "E100: requested unit (" << unit 
			 << ") does not exist!\n";
	return;
    }

    // Share control
    if(_devices[unit].in_use) {
	db<E100>(WRN) << "E100: device already in use!\n";
	return;
    }
    
    *this = *_devices[unit].device;

    // Lock device
    _devices[unit].in_use = true;
}

E100::~E100()
{
    db<E100>(TRC) << "~E100(unit=" << _unit << ")\n";

    // Unlock device
    _devices[_unit].in_use = false;
}

E100::E100(unsigned int unit, 
	   Log_Addr io_mem, IO_Irq irq, DMA_Buffer * dma_buf)
{
    db<E100>(TRC) << "E100(unit=" << unit << ",io=" << io_mem 
		  << ",irq=" << irq << ",dma=" << dma_buf << ")\n";

    _unit = unit;
    _io_mem = io_mem;
    _irq = irq;
    _dma_buf = dma_buf;

    // Distribute the DMA_Buffer allocated by init()






    // Reset device
    reset();
}

void E100::reset()
{
}

int E100::send(const Address & dst, const Protocol & prot,
		  const void * data, unsigned int size)
{
    db<E100>(TRC) << "E100::send(src=" << _address
		     << ",dst=" << dst
		     << ",prot=" << prot
		     << ",data=" << data
		     << ",size=" << size
		     << ")\n";

    // Wait for a free buffer

    // Assemble the Ethernet frame

    // Status must be set last, since it can trigger a send

    // Trigger an immediate send poll

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    return size;
}

int E100::receive(Address * src, Protocol * prot,
		     void * data, unsigned int size)
{
    // Wait for a frame in the ring buffer

    // Disassemble the Ethernet frame

    db<E100>(TRC) << "E100::receive(src=" << *src
		    << ",prot=" << *prot
		    << ",data=" << data
		    << ",size=" << size
		    << ")\n";


    // Release the buffer to the NIC

    _statistics.rx_packets++;
    _statistics.rx_bytes += size;

    return 0;
}

void E100::handle_int()
{
    CPU::int_disable();

    CPU::int_enable();
}

__END_SYS
