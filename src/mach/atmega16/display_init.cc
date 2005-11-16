// EPOS-- ATMega16_Display Initialization

#include <mach/atmega16/display.h>

__BEGIN_SYS

// Class initialization
int ATMega16_Display::init(System_Info * si)
{
    db<ATMega16_Display>(TRC) << "ATMega16_Display::init()\n";

    return 0;
}

__END_SYS
