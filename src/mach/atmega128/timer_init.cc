// EPOS-- ATMega128_Timer Initialization

#include <mach/atmega128/timer.h>

__BEGIN_SYS

// Class initialization
int ATMega128_Timer::init(System_Info * si)
{
    db<ATMega128_Timer>(TRC) << "ATMega128_Timer::init()\n";

    return 0;
}

__END_SYS
