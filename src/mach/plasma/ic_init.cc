// EPOS PLASMA_IC Initialization

#include <machine.h>

__BEGIN_SYS

// Class initialization
void PLASMA_IC::init()
{
    db<PLASMA_IC>(TRC) << "PLASMA_IC::init()\n";

    MIPS32::int_disable();
    disable();
    MIPS32::int_enable();
}

__END_SYS
