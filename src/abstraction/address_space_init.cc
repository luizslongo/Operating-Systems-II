// EPOS-- Address_Space Initialization

#include <address_space.h>

__BEGIN_SYS

int Address_Space::init(System_Info * si)
{
    db<Address_Space>(TRC) << "Address_Space::init()\n";

    return 0;
}

__END_SYS
