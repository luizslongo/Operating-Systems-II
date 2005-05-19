// EPOS-- Realtime Thread Abstraction Declarations

#ifndef __realtime_thread_h
#define __realtime_thread_h

#include <utility/handler.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

class Periodic_Thread: public Thread
{
public:
    Periodic_Thread(int (* entry)(), 
		    RTC::Microseconds period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    const Priority & priority = NORMAL,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, state, priority, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times) {}
    template<class T1>
    Periodic_Thread(int (* entry)(T1 a1), T1 a1,
		    RTC::Microseconds period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    const Priority & priority = NORMAL,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, state, priority, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times) {}
    template<class T1, class T2>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2, 
		    RTC::Microseconds period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    const Priority & priority = NORMAL,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, a2, state, priority, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times) {}
    template<class T1, class T2, class T3>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
		    RTC::Microseconds period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    const Priority & priority = NORMAL,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, a2, a3, state, priority, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times) {}

    static void wait_next() { self()->suspend(); }

private:
    Handler_Thread _handler;
    Alarm _alarm;
};

__END_SYS

#endif
