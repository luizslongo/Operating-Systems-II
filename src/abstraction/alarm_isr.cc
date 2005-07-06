// EPOS-- Alarm ISR Implementation

#include <display.h>
#include <alarm.h>
#include <thread.h>
#include <machine.h>
#include <system/kmalloc.h>

__BEGIN_SYS

// Explicit template instantiations
template void Machine::isr_wrapper<Alarm::timer_isr>();

void Alarm::timer_isr(void)
{
    CPU::int_disable();

    static Tick next; // counter for next event
    static Handler * handler; // next event's handler

    _elapsed++;
    
    if(Traits::visible) {
	Display display;
	int lin, col;
	display.position(&lin, &col);
	display.position(0, 79);
	display.putc(_elapsed);
	display.position(lin, col);
    }

    if(_master_ticks && ((_elapsed % _master_ticks) == 0))
	_master();
    
    if(next > 0)
	next--; 
    else {
	if(handler) {
	    (*handler)();
	    handler = 0;
	}
	if(!_requests.empty()) {
	    Queue::Element * e = _requests.remove();
	    Alarm * alarm = e->object();
	    next = e->rank();
	    handler = alarm->_handler;
	    if(alarm->_times != INFINITE)
		alarm->_times--;
	    if(alarm->_times) {
		e->rank(alarm->_ticks);
		_requests.insert(e);
	    }
	}
    }

    CPU::int_enable();
}

__END_SYS
