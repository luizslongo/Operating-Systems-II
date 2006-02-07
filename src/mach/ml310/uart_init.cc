// EPOS-- ML310 UART Mediator Initialization

#include <mach/ml310/uart.h>
#include <mach/ml310/machine.h>

__BEGIN_SYS

int ML310_UART::init(System_Info * si)
{
    db<ML310_UART>(TRC) << "ML310_UART::init()\n";
    //Enable interrupt...
    ML310_IC::enable(ML310_IC::INT_UART_INTR);

    return 0;
}

__END_SYS
