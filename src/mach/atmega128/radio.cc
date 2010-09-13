// EPOS ATMega128 Radio (CC1000) NIC Mediator Implementation

#include <system/kmalloc.h>
#include <mach/atmega128/machine.h>
#include <mach/atmega128/radio.h>

__BEGIN_SYS

// Class attributes
Radio::Device Radio::_devices[UNITS];

// Class Methods
void Radio::int_handler(unsigned int interrupt)
{
    Radio * dev = get(interrupt);

    db<Radio>(TRC) << "Radio::int_handler(int=" << interrupt
		    << ",dev=" << dev << ")\n";
    if(!dev)
	db<Radio>(WRN) << "Radio::int_handler: handler not found\n";
    else 
	dev->handle_int();
}

// Methods
Radio::Radio(unsigned int unit)
{
    db<Radio>(TRC) << "Radio(unit=" << unit << ")\n";

    // Share control
    if(unit >= UNITS) {
	db<Radio>(WRN) << "Radio: requested unit (" << unit 
			 << ") does not exist!\n";
	return;
    }

    // Share control
    if(_devices[unit].in_use) {
	db<Radio>(WRN) << "Radio: device already in use!\n";
	return;
    }
    
    *this = *_devices[unit].device;

    // Lock device
    _devices[unit].in_use = true;
}

Radio::~Radio()
{
    db<Radio>(TRC) << "~Radio(unit=" << _unit << ")\n";

    // Unlock device
    _devices[_unit].in_use = false;
}

Radio::Radio(unsigned int unit, 
		 int io_port, int irq, void * dma_buf)
{
    db<Radio>(TRC) << "Radio(unit=" << unit << ",io=" << io_port 
		     << ",irq=" << irq << ",dma=" << dma_buf << ")\n";

}

void Radio::reset()
{
}

int Radio::send(const Address & dst, const Protocol & prot,
		  const void * data, unsigned int size)
{
    db<Radio>(TRC) << "Radio::send(src=" << _address
		     << ",dst=" << dst
		     << ",prot=" << prot
		     << ",data=" << data
		     << ",size=" << size
		     << ")\n";

    return size;
}

int Radio::receive(Address * src, Protocol * prot,
		     void * data, unsigned int size)
{
    db<Radio>(TRC) << "Radio::receive(src=" << *src
		    << ",prot=" << *prot
		    << ",data=" << data
		    << ",size=" << size
		    << ")\n";

    return 0;
}

void Radio::handle_int()
{
    CPU::int_disable();

    CPU::int_enable();
}

__END_SYS
