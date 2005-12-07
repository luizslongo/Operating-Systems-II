// EPOS-- PC Display Common Package Implementation

#include <mach/pc/display.h>

__BEGIN_SYS

int Serial_Display::init(System_Info * si)
{
    db<Serial_Display>(TRC) << "Serial_Display::init()\n";

    return 0;
}

__END_SYS
