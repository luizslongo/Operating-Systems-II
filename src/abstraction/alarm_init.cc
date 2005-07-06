// EPOS-- Alarm Abstraction Initialization

#include <alarm.h>
#include <machine.h>

__BEGIN_SYS

// Prevent implicit template instantiations
extern template void Machine::isr_wrapper<Alarm::timer_isr>();

int Alarm::init(System_Info * si)
{
    db<Alarm>(TRC) << "Alarm::init()\n";

    CPU::int_disable();

    Machine::int_handler(Machine::INT_TIMER, 
			 &Machine::isr_wrapper<timer_isr>);

    _timer.frequency(FREQUENCY);
    _timer.enable();

    CPU::int_enable();
    return 0;
}

__END_SYS
