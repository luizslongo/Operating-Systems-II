// EPOS-- Network Abstraction Initialization

#include <network.h>

__BEGIN_SYS

int Network::init(System_Info * si)
{
    db<Init>(TRC) << "Network::init()\n";

    return 0;
}

__END_SYS
