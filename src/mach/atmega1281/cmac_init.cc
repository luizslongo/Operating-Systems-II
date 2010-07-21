// EPOS-- CMAC Init
#include <machine.h>
#include <mach/atmega1281/cmac.h>
#include <alarm.h>

__BEGIN_SYS

CMAC::event_handler* CMAC::alarm_ev_handler = 0;
//Handler_Function CMAC::alarm_handler = &(CMAC::alarm_handler_function);
volatile unsigned long CMAC::alarm_ticks_ms = 0;
unsigned long CMAC::alarm_event_time_ms = 0;
//Alarm CMAC::alarm(100000, &(CMAC::alarm_handler), -1); //min resolution is 10 ms
Timer_2 CMAC::timer(CMAC::alarm_handler_function);
//Semaphore CMAC::timer_int(0);

unsigned int CMAC::_sleeping_period = Traits<CMAC>::SLEEPING_PERIOD;

void CMAC::init(unsigned int n) {
	if (Traits<CMAC>::TIME_TRIGGERED){
		db<CMAC>(TRC) << "CMAC::init - creating state machine alarm\n";

		alarm_activate(&(CMAC::state_machine_handler), _sleeping_period);
	}

//	IC::int_vector(Machine::irq2int(IC::IRQ_TIMER2_COMPA), alarm_handler_function);
	timer.frequency(1000); //1 ms period(resolution)
	timer.reset();
	timer.enable();

	Pseudo_Random::seed(Traits<CMAC>::ADDRESS);//for CSMA - should be on its own init

	/*
	if(Traits<CMAC>::SM_STEP_DEBUG){
		IC::int_vector(IC::IRQ_IRQ6, sm_step_int_handler);
		IC::int_vector(IC::IRQ_IRQ7, sm_step_int_handler);

		IC::enable_external_int(IC::EXT_IRQ_6, IC::IRQ_RISING_EDGE);
		IC::enable_external_int(IC::EXT_IRQ_6, IC::IRQ_RISING_EDGE);
	}
	*/
}

 __END_SYS

