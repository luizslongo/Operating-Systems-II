// EPOS-- ATMega1281 Timer Mediator Initialization

#include <timer.h>
#include <ic.h>

__BEGIN_SYS

int ATMega1281_Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()\n";

    CPU::int_disable();

    if(Traits<ATMega1281_Timer_1>::enabled)
        ATMega1281_Timer_1::init();

    if(Traits<ATMega1281_Timer_3>::enabled)
        ATMega1281_Timer_3::init();

    CPU::int_enable();

    return 0;
}

int ATMega1281_Timer_1::init()
{
    IC::int_vector(IC::IRQ_TIMER0_COMPA, int_handler);
    IC::enable(IC::IRQ_TIMER0_COMPA);

    return 0;
}

int ATMega1281_Timer_2::init()
{
    IC::int_vector(IC::IRQ_TIMER, int_handler);
    IC::enable(IC::IRQ_TIMER);

    return 0;
}

int ATMega1281_Timer_3::init()
{
    IC::int_vector(IC::IRQ_TIMER3_COMPA, int_handler);
    IC::enable(IC::IRQ_TIMER3_COMPA);

    return 0;
}

__END_SYS
