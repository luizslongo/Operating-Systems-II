// EPOS-- PC Serial Display Mediator Initialization

#include <display.h>

__BEGIN_SYS

int Serial_Display::init(System_Info * si)
{
    db<Serial_Display>(TRC) << "Serial_Display::init()\n";

    return 0;
}

__END_SYS
