// EPOS-- PC PCI Mediator

#include <mach/pc/pci.h>

__BEGIN_SYS

int PC_PCI::init(System_Info * si)
{
    _phy_io_mem = si->pmm.io_mem;

    db<PC_PCI>(TRC) << "PC_PCI::init(_phy_io_mem=" << _phy_io_mem << ")\n";

    int ret = 0;

    IA32::int_disable();

    IA32::out8(0xCFB, 0x01);
    Reg32 tmp = IA32::in32(CONFADDR);
    IA32::out32(CONFADDR, 0x80000000);
    if(IA32::in32(CONFADDR) != 0x80000000)
	ret = -1;
    IA32::out32(CONFADDR, tmp);

    IA32::int_enable();

    return ret;
}

__END_SYS
