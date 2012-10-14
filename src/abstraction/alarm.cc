// EPOS Alarm Abstraction Implementation

#include <system/kmalloc.h>
#include <semaphore.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes
Alarm_Timer * Alarm::_timer;
volatile Alarm::Tick Alarm::_elapsed;
Alarm::Queue Alarm::_requests;

// Methods
Alarm::Alarm(const Microsecond & time, Handler handler, int times):
    _ticks(ticks(time)), _handler(handler), _times(times), _link(this, _ticks)
{
    lock();

    db<Alarm>(TRC) << "Alarm(t=" << time
                   << ",tk=" << _ticks
                   << ",h=" << reinterpret_cast<void *>(handler)
                   << ",x=" << times << ") => " << this << "\n";

    if(_ticks) {
        _requests.insert(&_link);
        unlock();
    } else {
        unlock();
        handler();
    }
}

Alarm::~Alarm()
{
    lock();

    db<Alarm>(TRC) << "~Alarm()\n";

    _requests.remove(this);

    unlock();
}

// Class methods
void Alarm::delay(const Microsecond & time)
{
    db<Alarm>(TRC) << "Alarm::delay(time=" << time << ")\n";

	Tick t = _elapsed + ticks(time);

	while(_elapsed < t);

}

void Alarm::handler()
{
    static Tick next_tick;
    static Handler next_handler;

    lock();

    _elapsed++;

    if(Traits<Alarm>::visible) {
        Display display;
        int lin, col;
        display.position(&lin, &col);
        display.position(0, 79);
        display.putc(_elapsed);
        display.position(lin, col);
    }

    if(next_tick)
        next_tick--;
    if(!next_tick) {
        if(next_handler) {
            db<Alarm>(TRC) << "Alarm::handler(h="
                           << reinterpret_cast<void *>(next_handler) << ")\n";
            unlock();
            next_handler();
            lock();
        }
        if(_requests.empty())
            next_handler = 0;
        else {
            Queue::Element * e = _requests.remove();
            Alarm * alarm = e->object();
            next_tick = alarm->_ticks;
            next_handler = alarm->_handler;
            if(alarm->_times != -1)
                alarm->_times--;
            if(alarm->_times) {
                e->rank(alarm->_ticks);
                _requests.insert(e);
            }
        }
    }

    unlock();
}

__END_SYS
