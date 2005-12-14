// EPOS-- PC NIC Mediator Initialization

#include <system/kmalloc.h>
#include <mach/pc/pc.h>
#include <mach/pc/pcnet32.h>

__BEGIN_SYS

int PC_NIC::init(System_Info * si)
{
    db<PC_NIC>(TRC) << "PC_NIC::init()\n";

    int status = 0;
    for(unsigned int i = 0; i < UNITS; i++) {
	// Scan the PCI bus searching for a NIC device
	PC_PCI::Locator loc = 
	    PC_PCI::scan(PCI_VENDOR_ID, PCI_DEVICE_ID, i);
	if(!loc) {
	    db<PC_NIC>(WRN) << "PC_NIC::init: PCI scan failed!\n";
	    status = 1;
	    continue;
	}

	// Try to enable IO and memory regions as well as bust master
	PC_PCI::command(loc, PC_PCI::command(loc) | PC_PCI::COMMAND_IO |
			PC_PCI::COMMAND_MEMORY | PC_PCI::COMMAND_MASTER);

	// Get the config space header and check it we got IO, MEM and MASTER
	PC_PCI::Header hdr;
	PCI::header(loc, &hdr);
	if(!hdr) {
	    db<PC_NIC>(WRN) << "PC_NIC::init: PCI header failed!\n";
	    status = 1;
	    continue;
	}
	db<PC_NIC>(INF) << "PC_NIC::init: PCI header=" << hdr << "}\n";
	if(!(hdr.command & PC_PCI::COMMAND_IO))
	    db<PC_NIC>(WRN) << "PC_NIC::init: I/O unaccessible!\n";
	if(!(hdr.command & PC_PCI::COMMAND_MEMORY))
	    db<PC_NIC>(WRN) << "PC_NIC::init: MEM unaccessible!\n";
	if(!(hdr.command & PC_PCI::COMMAND_MASTER))
	    db<PC_NIC>(WRN) << "PC_NIC::init: not master capable!\n";

	// Get I/O base port
	IO_Port io_port = 0;
	if(PCI_REG_IO < PCI::Region::N) {
	    if(!hdr.region[PCI_REG_IO].size || hdr.region[PCI_REG_IO].memory) {
		db<PC_NIC>(WRN) 
		    << "PC_NIC::init: not a valid I/O region!\n";
		status = 2;
		continue;
	    }
	    io_port = hdr.region[PCI_REG_IO].phy_addr;
	    db<PC_NIC>(INF) << "PC_NIC::init: I/O port at " 
			    << (void *)(int)io_port << "\n";
	}

	// Get I/O base address
	Phy_Addr io_mem = 0;
	if(PCI_REG_MEM < PCI::Region::N) {
	    if(!hdr.region[PCI_REG_MEM].size || 
	       !hdr.region[PCI_REG_MEM].memory) {
		db<PC_NIC>(WRN) 
		    << "PC_NIC::init: not a valid memory (I/O) region!\n";
		status = 2;
		continue;
	    }
	    io_mem = hdr.region[PCI_REG_MEM].phy_addr;
	    db<PC_NIC>(INF) << "PC_NIC::init: I/O memory at " 
			    << io_mem << "\n";
	}

	// Get I/O irq
	IO_Irq irq = hdr.interrupt_line;
	db<PC_NIC>(INF) << "PC_NIC::init: PCI interrut pin "
			<< hdr.interrupt_pin << " routed to IRQ "
			<< hdr.interrupt_line << "\n";

	// Allocate a DMA Buffer for init block, rx and tx rings
	DMA_Buffer * dma_buf = new(kmalloc(sizeof(MMU::DMA_Buffer))) 
	    DMA_Buffer(DMA_BUFFER_SIZE);

	// Initialize the device
	Device * nic = new (kmalloc(sizeof(Device)))
	    Device(io_port, io_mem, irq, dma_buf);

	// Register the device to the Machine
	new (kmalloc(sizeof(PC_Device)))
	    PC_Device(Type2Id<PC_NIC>::ID, i, nic, PC::irq2int(irq));

	// Install interrupt handler
	PC::int_vector(PC::irq2int(irq), &Device::int_handler);

	// Enable interrupts for device
	IC::enable(irq);
    }

    return status;
}

__END_SYS
