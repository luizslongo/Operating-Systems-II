// EPOS-- PC PCI Mediator

#include <mach/pc/pci.h>

__BEGIN_SYS

int PC_PCI::init(System_Info * si)
{
    int ret = 0;

    db<PC_PCI>(TRC) << "PC_PCI::init()\n";

    IA32::int_disable();
    IA32::out8(0xCFB, 0x01);
    IA32::Reg32 tmp = IA32::in32(0xCF8);
    IA32::out32(0xCF8, 0x80000000);
    if(IA32::in32(0xCF8) != 0x80000000)
	ret = -1;
    IA32::out32(0xCF8, tmp);
    
    IA32::int_disable();

    return ret;
}

__END_SYS
