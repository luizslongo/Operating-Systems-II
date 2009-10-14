// EPOS-- ATMega128 Timer Mediator Initialization

#include <timer.h>
#include <ic.h>

__BEGIN_SYS

int ATMega128_Timer::init()
{
    db<Init, Timer>(TRC) << "Timer::init()\n";

    CPU::int_disable();

    if(Traits<ATMega128_Timer_1>::enabled)
        ATMega128_Timer_1::init();

    if(Traits<ATMega128_Timer_3>::enabled)
        ATMega128_Timer_3::init();

    CPU::int_enable();

    return 0;
}

int ATMega128_Timer_1::init()
{
    IC::int_vector(IC::IRQ_TIMER, int_handler);
    IC::enable(IC::IRQ_TIMER);

    return 0;
}

int ATMega128_Timer_2::init()
{
    IC::int_vector(IC::IRQ_TIMER2_COMP, int_handler);
    IC::enable(IC::IRQ_TIMER2_COMP);

    return 0;
}

int ATMega128_Timer_3::init()
{
    IC::int_vector(IC::IRQ_TIMER3_COMPA, int_handler);
    IC::enable(IC::IRQ_TIMER3_COMPA);

    return 0;
}

__END_SYS
