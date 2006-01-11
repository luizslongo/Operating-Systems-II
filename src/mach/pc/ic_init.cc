// EPOS-- PC Interrupt Controller Initialization

#include <mach/pc/ic.h>

__BEGIN_SYS

int PC_IC::init(System_Info * si)
{
    db<Init, PC_IC>(TRC) << "PC_IC::init() => base=" << HARD_INT << "\n";

    CPU::int_disable();

    remap(HARD_INT);
    disable();

    CPU::int_enable();

    return 0;
}

__END_SYS
