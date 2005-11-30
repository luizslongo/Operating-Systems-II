// EPOS-- Alarm Abstraction Implementation

#include <display.h>
#include <alarm.h>
#include <thread.h>
#include <system/kmalloc.h>

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
      _times(times), _link(this, _ticks)
{
    db<Alarm>(TRC) << "Alarm(t=" << time 
		   << ",ticks=" << _ticks
		   << ",h=" << (void *)handler
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
    db<Alarm>(TRC) << "Alarm::master(t=" << time << ",h="
		   << (void *)handler << ")\n";

    _master = handler;
    _master_ticks = (time + period() / 2) / period();
}

void Alarm::delay(const Microseconds & time)
{
    db<Alarm>(TRC) << "Alarm::delay(t=" << time << ")\n";
    if (time > 0) 
        if(__SYS(Traits)<Thread>::idle_waiting) {
	    Handler_Thread handler(Thread::self());
	    Alarm alarm(time, &handler, 1);
	    Thread::self()->suspend();
        } else {
	    Tick t = _elapsed + time / period();
	    while(_elapsed < t);
        }
}

void Alarm::int_handler(unsigned int)
{
    // This is a very special interrupt handler, for the master alarm handler
    // called at the end might trigger a context switch (e.g. when it is set
    // to call the thread scheduler). In this case, int_handler won't finish
    // within the expected time (i.e., it will finish only when the preempted
    // thread return to the CPU). For this NOT to be an issue, the following
    // conditions MUST be met:
    // 1 - The interrupt dispatcher must acknowledge the handling of interrupts
    //     before invoking the respective handler, thus enabling subsequent
    //     interrupts to be handled even if a previous didn't come to an end
    // 2 - Handlers (e.g. master) must be called after incrementing _elapsed
    // 3 - The manipulation of alarm queue must be guarded (e.g. int_disable)

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

    if(_master_ticks && ((_elapsed % _master_ticks) == 0))
	_master();
}

__END_SYS
