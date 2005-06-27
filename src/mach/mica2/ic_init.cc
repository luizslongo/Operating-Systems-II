// EPOS-- Mica2_IC Initialization

#include <mach/mica2/ic.h>

__BEGIN_SYS

// Class initialization
int Mica2_IC::init(System_Info * si)
{
    db<Mica2_IC>(TRC) << "Mica2_IC::init()\n";

    return 0;
}

__END_SYS
