// EPOS CMAC Init

#include <machine.h>
#include <thread.h>

#ifdef __cmac_h

__BEGIN_SYS

template<> CMAC<Radio_Wrapper>::event_handler* CMAC<Radio_Wrapper>::alarm_ev_handler = 0;
template<> volatile unsigned long CMAC<Radio_Wrapper>::alarm_ticks_ms = 0;
template<> unsigned long CMAC<Radio_Wrapper>::alarm_event_time_ms     = 0;

template<> Timer_CMAC * CMAC<Radio_Wrapper>::_timer = 0;

template<> unsigned int CMAC<Radio_Wrapper>::_sleeping_period = Traits<CMAC<Radio_Wrapper> >::SLEEPING_PERIOD;


int workaround_for_time_triggered_cmac()
{
	if(Traits<CMAC<Radio_Wrapper> >::time_triggered)
		while(true) Thread::yield();
	return 0;
}


template<>
void CMAC<Radio_Wrapper>::init(unsigned int n) {
    Radio_Wrapper::init();

    if (Traits<CMAC<Radio_Wrapper> >::time_triggered) {
        db<CMAC<Radio_Wrapper> >(TRC) << "CMAC::init - creating state machine alarm\n";

        alarm_activate(&(state_machine_handler), _sleeping_period);

		/*******************************************************************************************/
		// This is a workaround for time trigger to work.
		// CMAC cannot run on a thread with lower priority than the thread waiting for CMAC,
		// so this prevents the IDLE thread from ever being scheduled.

			Thread * t = new (kmalloc(sizeof(Thread)))Thread(&workaround_for_time_triggered_cmac);
			Thread * tt = new (kmalloc(sizeof(Thread)))Thread(&workaround_for_time_triggered_cmac);
		/*******************************************************************************************/
    }

    _timer->frequency(500);
    _timer->reset();
    _timer->enable();


	if(Traits<CMAC<Radio_Wrapper> >::csma)
	    Pseudo_Random::seed(Traits<CMAC<Radio_Wrapper> >::ADDRESS); // for CSMA - should be on its own init
}

 __END_SYS

#endif
