// EPOS-- Chronometer Abstraction Initialization

#include <chronometer.h>

__BEGIN_SYS

int Chronometer::init(System_Info * si)
{
    db<Chronometer>(TRC) << "Chronometer::init()\n";

    return 0;
}

__END_SYS
