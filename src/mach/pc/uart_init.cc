// EPOS-- PC UART Mediator Initialization

#include <mach/pc/uart.h>

__BEGIN_SYS

int PC_UART::init(System_Info * si)
{
    db<PC_UART>(TRC) << "PC_UART::init()\n";

    return 0;
}

__END_SYS
