// EPOS-- Alarm Abstraction Implementation

#include <system/kmalloc.h>
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
Alarm::Alarm(const Microsecond & time, Handler * handler, int times)
    : _ticks((time + period() / 2) / period()), _handler(handler),
      _times(times), _link(this, _ticks)
{
    db<Alarm>(TRC) << "Alarm(t=" << time 
		   << ",ticks=" << _ticks
		   << ",h=" << (void *)handler
		   << ",x=" << times << ") => " << this << "\n";
    if(_ticks) {
	CPU::int_disable();
	_requests.insert(&_link);
	CPU::int_enable();
    } else
	(*handler)();
}

Alarm::Alarm(const Microsecond & time, Handler * handler, int times, 
	     bool int_enable)
    : _ticks((time + period() / 2) / period()), _handler(handler),
      _times(times), _link(this, _ticks)
{
    db<Alarm>(TRC) << "Alarm(t=" << time 
		   << ",ticks=" << _ticks
		   << ",h=" << (void *)handler
		   << ",x=" << times << ") => " << this << "\n";
    if(_ticks) {
	CPU::int_disable();
	_requests.insert(&_link);
	if(int_enable)
	    CPU::int_enable();
    } else
	(*handler)();
}

Alarm::~Alarm() {
    db<Alarm>(TRC) << "~Alarm()\n";

    CPU::int_disable();
    _requests.remove(this);
    CPU::int_enable();
}

void Alarm::master(const Microsecond & time, Handler::Function * handler)
{
    db<Alarm>(TRC) << "Alarm::master(t=" << time << ",h="
		   << (void *)handler << ")\n";

    _master = handler;
    _master_ticks = (time + period() / 2) / period();
}

void Alarm::delay(const Microsecond & time)
{
    db<Alarm>(TRC) << "Alarm::delay(t=" << time << ")\n";
    if(time > 0) 
        if(__SYS(Traits)<Thread>::idle_waiting) {
	    CPU::int_disable();
	    Handler_Thread handler(Thread::self());
	    Alarm alarm(time, &handler, 1, false);
	    Thread::self()->suspend();
	    CPU::int_enable();
        } else {
	    Tick t = _elapsed + (time + period() / 2) / period();
	    while(_elapsed < t)
		Thread::yield();
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

    static volatile bool busy = false; // no event to care for
    static volatile Tick next;         // ticks until next event
    static Handler * volatile handler; // next event's handler

    CPU::int_disable();

    Handler * current = 0;

    _elapsed++;
    
    if(visible) {
	Display display;
	int lin, col;
	display.position(&lin, &col);
	display.position(0, 79);
	display.putc(_elapsed);
	display.position(lin, col);
    }

    if(busy) {
	if(next > 0)
	    next--; 
	else {
	    db<Alarm>(TRC) << "Alarm::handler(h=" << handler << ")\n";
	    current = handler;
	    busy = false;
	}
    }

    if(!busy && !_requests.empty()) {
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
	busy = true;
    }

    CPU::int_enable();

    if(_master_ticks && ((_elapsed % _master_ticks) == 0)) 
	_master();
    
    if(current) 
	(*current)();
}

__END_SYS
