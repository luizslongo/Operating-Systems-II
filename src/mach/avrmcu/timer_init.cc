// EPOS-- AVRMCU_Timer Initialization

#include <mach/avrmcu/timer.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_Timer::init(System_Info * si)
{
    db<AVRMCU_Timer>(TRC) << "AVRMCU_Timer::init()\n";

    return 0;
}

__END_SYS
