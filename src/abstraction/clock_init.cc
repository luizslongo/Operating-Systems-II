// EPOS-- Clock Abstraction Initialization

#include <clock.h>

__BEGIN_SYS

int Clock::init(System_Info * si)
{
    db<Clock>(TRC) << "Clock::init()\n";

    return 0;
}

__END_SYS
