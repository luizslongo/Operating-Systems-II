// EPOS-- PC Interrupt Controller Initialization

#include <ic.h>

__BEGIN_SYS

void PC_IC::init()
{
    db<Init, PC_IC>(TRC) << "PC_IC::init(base=" << HARD_INT << ")\n";

    CPU::int_disable();

    remap(HARD_INT);
    disable();

    CPU::int_enable();
}

__END_SYS
