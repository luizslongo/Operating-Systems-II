// EPOS Alarm Abstraction Implementation

#include <system/kmalloc.h>
#include <semaphore.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
Spin Alarm_Base::_lock;
Alarm_Timer * Single_Core_Alarm::_timer;
volatile Single_Core_Alarm::Tick Single_Core_Alarm::_elapsed;
Single_Core_Alarm::Queue Single_Core_Alarm::_requests;

// Methods
Single_Core_Alarm::Single_Core_Alarm(const Microsecond & time, Handler & handler, int times):
Alarm_Base(time, handler, times), _link(this, _ticks)
{
    lock();

    db<Single_Core_Alarm>(TRC) << "Alarm(t=" << time
    << ",tk=" << _ticks
    << ",h=" << (void *)handler
    << ",x=" << times << ") => " << this << "\n";

    if(_ticks) {
        _requests.insert(&_link);
        unlock();
    } else {
        unlock();
        handler();
    }
}

Single_Core_Alarm::~Single_Core_Alarm()
{
    lock();

    db<Single_Core_Alarm>(TRC) << "~Single_Core_Alarm()\n";

    _requests.remove(this);

    unlock();
}


// Class methods
void Single_Core_Alarm::delay(const Microsecond & time)
{
    db<Single_Core_Alarm>(TRC) << "Single_Core_Alarm::delay(time=" << time << ")\n";


	Tick t = _elapsed + ticks(time);

	while(_elapsed < t);

}


void Single_Core_Alarm::handler()
{
    CPU::int_disable();
    // lock(); this handler is meant to be called obly by CPU[0]

    _elapsed++;

    Single_Core_Alarm * alarm = 0;

    if(!_requests.empty()) {
	// rank can be negative whenever multiple handlers get created for the same time tick
        if(_requests.head()->promote() <= 0) {

            Queue::Element * e = _requests.remove();
            alarm = e->object();

            if(alarm->_times != INFINITE)
                alarm->_times--;

            if(alarm->_times) {
                e->rank(alarm->_ticks);
                _requests.insert(e);
            }
        }
    }

    // unlock();
    CPU::int_enable();

    if(alarm) {
	db<Single_Core_Alarm>(TRC) << "Single_Core_Alarm::handler(h=" << reinterpret_cast<void*>(alarm->handler) << ")\n";
	(*alarm->_handler)();
    }
}

__END_SYS
