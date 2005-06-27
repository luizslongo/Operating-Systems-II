// EPOS-- Mica2_Timer Initialization

#include <mach/mica2/timer.h>

__BEGIN_SYS

// Class initialization
int Mica2_Timer::init(System_Info * si)
{
    db<Mica2_Timer>(TRC) << "Mica2_Timer::init()\n";

    return 0;
}

__END_SYS
