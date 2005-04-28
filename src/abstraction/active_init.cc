// EPOS-- Active Object Abstraction Initialization

#include <active.h>

__BEGIN_SYS

int Active::init(System_Info * si)
{
    db<Init>(TRC) << "Active::init()\n";

    return 0;
}

__END_SYS
