// EPOS-- Mica2_Display Initialization

#include <mach/mica2/display.h>

__BEGIN_SYS

// Class initialization
int Mica2_Display::init(System_Info * si)
{
    db<Mica2_Display>(TRC) << "Mica2_Display::init()\n";

    return 0;
}

__END_SYS
