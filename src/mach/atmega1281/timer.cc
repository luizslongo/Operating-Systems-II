// EPOS-- ATMega1281 Timer Mediator Implementation

#include <machine.h>
#include <timer.h>

__BEGIN_SYS

// Class attributes
Handler::Function * ATMega1281_Timer_1::_handler;
Handler::Function * ATMega1281_Timer_2::_handler;
Handler::Function * ATMega1281_Timer_3::_handler;

// Class methods
void ATMega1281_Timer_1::int_handler(unsigned int i)
{
    _handler();
}

void ATMega1281_Timer_2::int_handler(unsigned int i)
{
    _handler();
}

void ATMega1281_Timer_3::int_handler(unsigned int i)
{
    _handler();
}

__END_SYS
