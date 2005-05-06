// EPOS-- Alarm Abstraction Declarations

#ifndef __alarm_h
#define __alarm_h

#include <system/config.h>
#include <utility/queue.h>
#include <utility/handler.h>
#include <tsc.h>
#include <rtc.h>
#include <timer.h>

__BEGIN_SYS

class Alarm
{
private:
    typedef Traits<Alarm> Traits;
    static const Type_Id TYPE = Type<Alarm>::TYPE;

    static const int FREQUENCY = __SYS(Traits)<Timer>::FREQUENCY;

    typedef Relative_Queue<Alarm, __SYS(Traits)<Thread>::smp> Queue;

    typedef TSC::Hertz Hertz;
    typedef TSC::Time_Stamp Time_Stamp;
    typedef RTC::Microseconds Microseconds;
    typedef RTC::Seconds Seconds;
    typedef Timer::Tick Tick;

public:
    // Infinite times (for alarms)
    enum { INFINITE = -1 };

public:
    Alarm(const Microseconds & time, Handler * handler, int times = 1);
    ~Alarm();

    static void master(const Microseconds & time, Handler::Function * handler);

    static Hertz frequency() {return _timer.frequency(); }

    static void delay(const Microseconds & time);

    static int init(System_Info * si);

private:
    static Microseconds period() { return 1000000 / frequency(); }
    static void timer_handler(void);

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
