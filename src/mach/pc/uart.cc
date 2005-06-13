// EPOS-- PC UART Mediator

#include <mach/pc/uart.h>

__BEGIN_SYS

const PC_UART::IO_Port PC_UART::_ports[] = { Traits::COM1, 
					     Traits::COM2, 
					     Traits::COM3,
					     Traits::COM4 };

__END_SYS
