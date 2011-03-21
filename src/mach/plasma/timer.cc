// EPOS PLASMA Timer Mediator Implementation

#include <machine.h>
#include <timer.h>

__BEGIN_SYS

// Class attributes
PLASMA_Timer::Count PLASMA_Timer::_count = (1<<18); //fixed by hardware
PLASMA_Timer * PLASMA_Timer::_channels[CHANNELS];

// Class methods
void PLASMA_Timer::int_handler(unsigned int i) 
{
	db<PLASMA_Timer>(TRC) << "<Timer::int_handler>";

	//Always calls ALARM Channel ?
	if(_channels[ALARM])
		_channels[ALARM]->_handler();
	
	//Check if SCHEDULER channel must be handled.
	if(_channels[SCHEDULER] && (--_channels[SCHEDULER]->_current <= 0)){
		_channels[SCHEDULER]->_current = _channels[SCHEDULER]->_initial;
		_channels[SCHEDULER]->_handler();
	}

}

__END_SYS
