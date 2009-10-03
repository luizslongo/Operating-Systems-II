// EPOS-- PC Timer Mediator Implementation

#include <machine.h>
#include <timer.h>

__BEGIN_SYS

// Class attributes
PC_Timer * PC_Timer::_channels[CHANNELS];

// Class methods
void PC_Timer::int_handler(unsigned int i)
{
    if(Machine::cpu_id() == 0) {
	if(_channels[ALARM])
	    _channels[ALARM]->_handler();

	if(_channels[USER] && _channels[USER]->_enabled
	   && (--_channels[USER]->_current <= 0)) {
	    _channels[USER]->_current = _channels[USER]->_initial;
	    _channels[USER]->_handler();
	}
    }

    if(_channels[SCHEDULER] && _channels[SCHEDULER]->_enabled
       && (--_channels[SCHEDULER]->_current <= 0)) {
	_channels[SCHEDULER]->_current = _channels[SCHEDULER]->_initial;
	_channels[SCHEDULER]->_handler();
    }
}

__END_SYS
