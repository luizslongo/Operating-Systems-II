// EPOS AIX4LITE Timer Mediator Initialization

#include <timer.h>
#include <ic.h>

__BEGIN_SYS

void AIX4LITE_Timer::init() {
    
	db<Init, Timer>(TRC) << "AIX4LITE_Timer::init()\n";

    CPU::int_disable();

	IC::disable(IC::IRQ_TIMER);
	IC::int_vector(IC::IRQ_TIMER, &AIX4LITE_Timer::int_handler);
	
	//Reprogram the Timer
	*_timer_reg = *_timer_reg + (CLOCK / FREQUENCY);
	
	IC::enable(IC::IRQ_TIMER);
	CPU::int_enable();
}

__END_SYS

