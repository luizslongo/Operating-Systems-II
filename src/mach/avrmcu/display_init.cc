// EPOS-- AVRMCU_Display Initialization

#include <mach/avrmcu/display.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_Display::init(System_Info * si)
{
    db<AVRMCU_Display>(TRC) << "AVRMCU_Display::init()\n";

    return 0;
}

__END_SYS
