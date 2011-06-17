// EPOS AIX4LITE UART Mediator Implementation

#include <uart.h>

__BEGIN_SYS

AIX4LITE_UART::MM_Reg * AIX4LITE_UART::_regs = reinterpret_cast<AIX4LITE_UART::MM_Reg *>(AIX4LITE_UART::BASE_ADDRESS);

void AIX4LITE_UART::int_handler(unsigned int interrupt){
    db<AIX4LITE_UART>(TRC) << "AIX4LITE_UART::int_handler(intr=" << interrupt << ")\n";
	// To be implemented.
	
}

__END_SYS
