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

__END_SYS
