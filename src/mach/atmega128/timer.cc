// EPOS-- ATMega128 Timer Mediator Implementation

#include <machine.h>
#include <timer.h>

__BEGIN_SYS

// Class attributes
Handler::Function * ATMega128_Timer_1::_handler;
Handler::Function * ATMega128_Timer_2::_handler;
Handler::Function * ATMega128_Timer_3::_handler;

// Class methods
void ATMega128_Timer_1::int_handler(unsigned int i)
{
    _handler();
}

void ATMega128_Timer_2::int_handler(unsigned int i)
{
    _handler();
}

void ATMega128_Timer_3::int_handler(unsigned int i)
{
    _handler();
}

__END_SYS
