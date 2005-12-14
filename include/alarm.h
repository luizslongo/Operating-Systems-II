// EPOS-- Alarm Abstraction Declarations

#ifndef __alarm_h
#define __alarm_h

#include <utility/queue.h>
#include <utility/handler.h>
#include <tsc.h>
#include <rtc.h>
#include <timer.h>

__BEGIN_SYS

class Alarm
{
private:
    typedef TSC::Time_Stamp Time_Stamp;
    typedef Timer::Tick Tick;

    typedef Relative_Queue<Alarm, Traits<Thread>::smp> Queue;

    static const int FREQUENCY = Traits<Timer>::FREQUENCY;

    static const bool visible = Traits<Alarm>::visible;

public:
    typedef TSC::Hertz Hertz;
    typedef RTC::Microsecond Microsecond;

    // Infinite times (for alarms)
    enum { INFINITE = -1 };

public:
    Alarm(const Microsecond & time, Handler * handler, int times = 1);
    ~Alarm();

    static void master(const Microsecond & time, Handler::Function * handler);
    static Hertz frequency() {return _timer.frequency(); }
    static void delay(const Microsecond & time);

    static void int_handler(unsigned int irq);

    static int init(System_Info * si);

private:
    Alarm(const Microsecond & time, Handler * handler, int times,
	  bool int_enable);

    static Microsecond period() { return 1000000 / frequency(); }

private:
    Tick _ticks;
    Handler * _handler;
    int _times;
    Queue::Element _link;

    static Timer _timer;
    static volatile Tick _elapsed;
    static Handler::Function * _master;
    static Tick _master_ticks;
    static Queue _requests;
};

__END_SYS

#endif
