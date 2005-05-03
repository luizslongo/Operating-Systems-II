// EPOS-- Realtime Thread Abstraction Declarations

#ifndef __realtime_thread_h
#define __realtme_thread_h

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

    static void wait_next() { running()->suspend(); }

private:
    Handler_Thread _handler;
    Alarm _alarm;
};

__END_SYS

#endif
