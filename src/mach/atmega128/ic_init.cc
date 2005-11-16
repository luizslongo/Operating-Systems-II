// EPOS-- ATMega128_IC Initialization

#include <mach/atmega128/ic.h>

__BEGIN_SYS

// Class initialization
int ATMega128_IC::init(System_Info * si)
{
    db<ATMega128_IC>(TRC) << "ATMega128_IC::init()\n";

    return 0;
}

__END_SYS
