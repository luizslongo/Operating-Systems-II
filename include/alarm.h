// EPOS Alarm Abstraction Declarations

#ifndef __alarm_h
#define __alarm_h

#include <utility/queue.h>
#include <tsc.h>
#include <rtc.h>
#include <timer.h>
#include <semaphore.h>

__BEGIN_SYS

class Alarm_Base
{
public:
    typedef TSC::Time_Stamp Time_Stamp;
    typedef Timer::Tick Tick;  
    
    // An alarm handler
    typedef void (* Handler)();

    static const bool smp = Traits<Thread>::smp;
    
public:
    typedef TSC::Hertz Hertz;
    typedef RTC::Microsecond Microsecond;

    // Infinite times (for alarms)
    enum { INFINITE = -1 };
    
     static Hertz resolution() { return Alarm_Timer::FREQUENCY; }
     
public:
    Alarm_Base(const Microsecond & time, Handler & handler, int times) :
    _ticks(ticks(time)), _handler(handler), _times(times) { }
  
    static Microsecond period() {
        return 1000000 / resolution();
    }

    static Tick ticks(const Microsecond & time) {
        return (time + period() / 2) / period();
    }

    static void lock() {
        CPU::int_disable();
        if(smp)
            _lock.acquire();
    }

    static void unlock() {
        if(smp)
            _lock.release();
        CPU::int_enable();
    }
    
public:
    Tick _ticks;
    Handler _handler;
    int _times; 
    static Spin _lock;
  
};

class Single_Core_Alarm : public Alarm_Base
{
private:
    typedef Relative_Queue<Single_Core_Alarm, Tick> Queue;

public:    
    Single_Core_Alarm(const Microsecond & time, Handler & handler, int times = 1);
    ~Single_Core_Alarm();

    static void delay(const Microsecond & time);

    static int init();

private:
    static void handler();

private:
    Queue::Element _link; 
  
    static Alarm_Timer * _timer;
    static volatile Tick _elapsed;
    static Queue _requests;
};



//Define the Alarm that will be used by the rest of the system
typedef Single_Core_Alarm Alarm;


__END_SYS

#endif
