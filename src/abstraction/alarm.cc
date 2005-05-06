// EPOS-- Alarm Abstraction Implementation

#include <display.h>
#include <alarm.h>
#include <thread.h>

__BEGIN_SYS

// Class attributes
Timer Alarm::_timer;
volatile Alarm::Tick Alarm::_elapsed;
Handler::Function * Alarm::_master;
Alarm::Tick Alarm::_master_ticks;
Alarm::Queue Alarm::_requests;

// Methods
Alarm::Alarm(const Microseconds & time, Handler * handler, int times)
    : _ticks((time + period() / 2) / period()), _handler(handler),
      _times(times), _link(this, (int)_ticks)
{
    db<Alarm>(TRC) << "Alarm(t=" << time << ",h=" << (void *)handler
		   << ",x=" << times << ")\n";
    if(!_ticks) {
	(*handler)();
	return;
    }

    CPU::int_disable();
    _requests.insert(&_link);
    CPU::int_enable();
}

Alarm::~Alarm() {
    db<Alarm>(TRC) << "~Alarm()\n";
    CPU::int_disable();
    _requests.remove(this);
    CPU::int_enable();
}

void Alarm::master(const Microseconds & time, Handler::Function * handler)
{
    db<Alarm>(TRC) << "master(t=" << time << ",h="
		   << (void *)handler << ")\n";

    _master = handler;
    _master_ticks = (time + period() / 2) / period();
}

void Alarm::delay(const Microseconds & time)
{
    db<Alarm>(TRC) << "delay(t=" << time << ")\n";
    Tick t = _elapsed + time / period();
    while(_elapsed < t)
	if(__SYS(Traits)<Thread>::idle_waiting)
	    Thread::yield();
}

void Alarm::timer_handler(void)
{
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
}

__END_SYS
