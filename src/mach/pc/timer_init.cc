// EPOS-- PC Timer Mediator

#include <mach/pc/timer.h>

__BEGIN_SYS

int PC_Timer::init(System_Info * si)
{
    db<PC_Timer>(TRC) << "PC_Timer::init()\n";

    return 0;
}

__END_SYS
