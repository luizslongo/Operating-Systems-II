// EPOS Alarm Abstraction Declarations

#ifndef __alarm_h
#define __alarm_h

#include <utility/queue.h>
#include <utility/handler.h>
#include <tsc.h>
#include <rtc.h>
#include <timer.h>
#include <semaphore.h>

__BEGIN_SYS

class Alarm
{
public:
    typedef TSC::Time_Stamp Time_Stamp;
    typedef TSC::Hertz Hertz;
    typedef Timer::Tick Tick;  
    typedef RTC::Microsecond Microsecond;
    
    // An alarm handler
    typedef Function_Handler Handler;

    // Infinite times (for alarms)
    enum { INFINITE = -1 };
    
private:
    typedef Relative_Queue<Alarm, Tick> Queue;

public:
    Alarm(const Microsecond & time, Handler handler, int times = 1);
    ~Alarm();

    static Hertz frequency() { return _timer->frequency(); }

    static void delay(const Microsecond & time);

    static int init();

private:
    static Microsecond period() {
        return 1000000 / frequency();
    }

    static Tick ticks(const Microsecond & time) {
        return (time + period() / 2) / period();
    }

    static void lock() { Thread::lock(); }
    static void unlock() { Thread::unlock(); }

    static void handler(void);

private:
    Tick _ticks;
    Handler _handler;
    int _times; 
    Queue::Element _link;

    static Alarm_Timer * _timer;
    static volatile Tick _elapsed;
    static Queue _requests;
};

class Delay
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Delay(const Microsecond & time): _time(time)  { Alarm::delay(_time); }

private:
    Microsecond _time;

};

__END_SYS

#endif
