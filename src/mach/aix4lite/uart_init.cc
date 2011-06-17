// EPOS AIX4LITE UART Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void AIX4LITE_UART::init()
{
    db<AIX4LITE_UART>(TRC) << "AIX4LITE_UART::init()\n";

	//Set configured BAUDRATE;
	_regs->clkdiv = CLOCK/BAUDRATE;

}

__END_SYS
