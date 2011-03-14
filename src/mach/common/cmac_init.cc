// EPOS CMAC Init

#include <machine.h>

#ifdef __cmac_h

__BEGIN_SYS

template<> CMAC<Radio_Wrapper>::event_handler* CMAC<Radio_Wrapper>::alarm_ev_handler = 0;
template<> volatile unsigned long CMAC<Radio_Wrapper>::alarm_ticks_ms = 0;
template<> unsigned long CMAC<Radio_Wrapper>::alarm_event_time_ms     = 0;

//Function_Handler CMAC::func_handler = Function_Handler((CMAC::alarm_handler_function));
//Alarm CMAC::alarm(100000, &CMAC::func_handler, -1); //min resolution is 10 ms
template<> Timer_1 * CMAC<Radio_Wrapper>::_timer = 0;//(CMAC<Radio_Wrapper>::alarm_handler_function);

template<> unsigned int CMAC<Radio_Wrapper>::_sleeping_period = Traits<CMAC<Radio_Wrapper> >::SLEEPING_PERIOD;

template<>
void CMAC<Radio_Wrapper>::init(unsigned int n) {
    Radio_Wrapper::init();

    if (Traits<CMAC<Radio_Wrapper> >::TIME_TRIGGERED) {
        db<CMAC<Radio_Wrapper> >(TRC) << "CMAC::init - creating state machine alarm\n";

        alarm_activate(&(state_machine_handler), _sleeping_period);
    }

    _timer->frequency(500);
    _timer->reset();
    _timer->enable();

    Pseudo_Random::seed(Traits<CMAC<Radio_Wrapper> >::ADDRESS); // for CSMA - should be on its own init
}

 __END_SYS

#endif
