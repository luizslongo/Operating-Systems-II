// EPOS-- Condition Abstraction Initialization

#include <condition.h>

__BEGIN_SYS

int Condition::init(System_Info * si)
{
    db<Condition>(TRC) << "Condition::init()\n";

    return 0;
}

__END_SYS
