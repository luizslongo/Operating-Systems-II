// EPOS-- ATMega128_Display Initialization

#include <mach/atmega128/display.h>

__BEGIN_SYS

// Class initialization
int ATMega128_Display::init(System_Info * si)
{
    db<ATMega128_Display>(TRC) << "ATMega128_Display::init()\n";

    return 0;
}

__END_SYS
