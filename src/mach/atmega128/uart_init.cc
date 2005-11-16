// EPOS-- ATMega128_UART Initialization

#include <uart.h>

__BEGIN_SYS

// Class initialization
int ATMega128_UART::init(System_Info * si)
{
    db<ATMega128_UART>(TRC) << "ATMega128_UART::init()\n";
    
    return 0;
}

__END_SYS
