// EPOS-- ATMega16_Timer Initialization

#include <mach/atmega16/timer.h>

__BEGIN_SYS

// Class initialization
int ATMega16_Timer::init(System_Info * si)
{
    db<ATMega16_Timer>(TRC) << "ATMega16_Timer::init()\n";

    return 0;
}

__END_SYS
