// EPOS-- AVRMCU_IC Initialization

#include <mach/avrmcu/ic.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_IC::init(System_Info * si)
{
    db<AVRMCU_IC>(TRC) << "AVRMCU_IC::init()\n";

    return 0;
}

__END_SYS
