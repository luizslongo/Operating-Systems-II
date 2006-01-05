// EPOS-- PC PCI Mediator

#include <mach/pc/pci.h>

__BEGIN_SYS

int PC_PCI::init(System_Info * si)
{
    _phy_io_mem = si->pmm.io_mem;

    db<Init, PC_PCI>(TRC) << "PC_PCI::init(pmm.io_mem=" 
			  << _phy_io_mem << ")\n";

    int ret = 0;

    CPU::int_disable();

    CPU::out8(0xCFB, 0x01);
    Reg32 tmp = CPU::in32(CONFADDR);
    CPU::out32(CONFADDR, 0x80000000);
    if(CPU::in32(CONFADDR) != 0x80000000)
	ret = -1;
    CPU::out32(CONFADDR, tmp);

    CPU::int_enable();

    return ret;
}

__END_SYS
