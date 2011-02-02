// EPOS CMAC Init

#include <machine.h>
#include <alarm.h>
#include <utility/handler.h>

#ifdef __cmac_h

__BEGIN_SYS

const CMAC::Address CMAC::BROADCAST = CMAC::Address(Traits<CMAC>::BROADCAST);
CMAC::event_handler* CMAC::alarm_ev_handler = 0;
volatile unsigned long CMAC::alarm_ticks_ms = 0;
unsigned long CMAC::alarm_event_time_ms     = 0;

//Function_Handler CMAC::func_handler = Function_Handler((CMAC::alarm_handler_function));
//Alarm CMAC::alarm(100000, &CMAC::func_handler, -1); //min resolution is 10 ms
Timer_1 CMAC::timer(CMAC::alarm_handler_function);

unsigned int CMAC::_sleeping_period = Traits<CMAC>::SLEEPING_PERIOD;

void CMAC::init(unsigned int n) {
    Radio::init();

    if (Traits<CMAC>::TIME_TRIGGERED) {
        db<CMAC>(TRC) << "CMAC::init - creating state machine alarm\n";

        alarm_activate(&(CMAC::state_machine_handler), _sleeping_period);
    }

    timer.frequency(500);
    timer.reset();
    timer.enable();

    Pseudo_Random::seed(Traits<CMAC>::ADDRESS); // for CSMA - should be on its own init
}

 __END_SYS

#endif
