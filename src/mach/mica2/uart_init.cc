// EPOS-- Mica2_UART Initialization

#include <uart.h>

__BEGIN_SYS

// Class initialization
int Mica2_UART::init(System_Info * si)
{
    db<Mica2_UART>(TRC) << "Mica2_UART::init()\n";
    
    return 0;
}

__END_SYS
