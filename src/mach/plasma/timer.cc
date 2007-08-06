// EPOS-- PLASMA Timer Mediator Implementation

#include <mach/plasma/timer.h>

__BEGIN_SYS

PLASMA_Timer::Count PLASMA_Timer::_count = (1<<18);  //fixed by hardware

void PLASMA_Timer::int_handler(unsigned int interrupt) {
	//will be overrided by alarm
	db<PLASMA_Timer>(TRC) << "<Timer::int_handler>";
}

__END_SYS
