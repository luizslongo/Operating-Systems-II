// EPOS-- Alarm Abstraction Initialization

#include <alarm.h>
#include <machine.h>

__BEGIN_SYS

int Alarm::init(System_Info * si)
{
    db<Alarm>(TRC) << "Alarm::init()\n";

    CPU::int_disable();

    Machine::int_handler(Machine::INT_TIMER,
			 &Machine::handler_wrapper<timer_handler>);

    _timer.frequency(FREQUENCY);
    _timer.enable();

    CPU::int_enable();
    return 0;
}

__END_SYS
