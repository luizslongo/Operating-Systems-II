// EPOS-- AVRMCU_UART Initialization


#include <uart.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_UART::init(System_Info * si)
{
    db<AVRMCU_UART>(TRC) << "AVRMCU_UART::init()\n";

    return 0;
}

__END_SYS
