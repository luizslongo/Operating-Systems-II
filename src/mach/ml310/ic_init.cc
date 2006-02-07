// EPOS-- ML310_IC Initialization

#include <mach/ml310/ic.h>
#include <mach/ml310/machine.h>

__BEGIN_SYS

// Class initialization
int ML310_IC::init(System_Info * si)
{
    db<ML310_IC>(TRC) << "ML310_IC::init()\n";

    PPC32::int_disable();

    //Master Enable and Hardware Generation of Interrupts !!!
    *(volatile unsigned int*)(MER) = 0x00000003;

    //Disable All Interrupts (Will be enabled on each HW init)
    disable();

    PPC32::int_enable();

    return 0;
}

__END_SYS
