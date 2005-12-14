// EPOS-- Alarm Abstraction Initialization

#include <alarm.h>
#include <machine.h>

__BEGIN_SYS

int Alarm::init(System_Info * si)
{
    db<Alarm>(TRC) << "Alarm::init()\n";

    CPU::int_disable();

    Machine::int_vector(Machine::irq2int(IC::IRQ_TIMER), int_handler);

    _timer.frequency(FREQUENCY);
    
    IC::enable(IC::IRQ_TIMER);

    CPU::int_enable();

    return 0;
}

__END_SYS
