// EPOS-- Periodic Thread Abstraction Declarations

#ifndef __periodic_thread_h
#define __periodic_thread_h

#include <utility/handler.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

// Aperiodic Thread
typedef Thread Aperiodic_Thread;

// Periodic Thread
class Periodic_Thread: public Thread
{
protected:
    typedef RTC::Microsecond Microsecond;

public:
    Periodic_Thread(int (* entry)(), 
		    const Microsecond & period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, BEGINNING, period, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times)
    {
	_state = state;
	if(state == READY)
	    _scheduler.resume(this); 
    }
    template<class T1>
    Periodic_Thread(int (* entry)(T1 a1), T1 a1,
		    const Microsecond & period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, BEGINNING, period, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times) 
    {
	_state = state;
	if(state == READY)
	    _scheduler.resume(this); 
    }
    template<class T1, class T2>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2, 
		    const Microsecond & period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, a2, BEGINNING, period, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times)

    {
	_state = state;
	if(state == READY)
	    _scheduler.resume(this); 
    }
    template<class T1, class T2, class T3>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
		    const Microsecond & period,
		    int times = Alarm::INFINITE,
		    const State & state = READY,
		    unsigned int stack_size = STACK_SIZE)
	: Thread(entry, a1, a2, a3, BEGINNING, period, stack_size),
	  _handler(this),
	  _alarm(period, &_handler, times)
    {
	_state = state;
	if(state == READY)
	    _scheduler.resume(this); 
    }

    static void wait_next() { self()->suspend(); }

private:
    Handler_Thread _handler;
    Alarm _alarm;
};

__END_SYS

#endif
