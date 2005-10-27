// EPOS-- PC Interrupt Controller Initialization

#include <mach/pc/pc.h>
#include <mach/pc/ic.h>

__BEGIN_SYS

int PC_IC::init(System_Info * si)
{
    db<PC_IC>(TRC) << "PC_IC::init()\n";

    PC_IC ic;

    ic.remap(HARD_INT);
    ic.disable();

    return 0;
}

__END_SYS
