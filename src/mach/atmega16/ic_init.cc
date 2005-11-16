// EPOS-- ATMega16_IC Initialization

#include <mach/atmega16/ic.h>

__BEGIN_SYS

// Class initialization
int ATMega16_IC::init(System_Info * si)
{
    db<ATMega16_IC>(TRC) << "ATMega16_IC::init()\n";

    return 0;
}

__END_SYS
