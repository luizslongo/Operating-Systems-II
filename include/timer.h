// EPOS-- Timer Mediator Common Package

#ifndef __timer_h
#define __timer_h

#include <utility/handler.h>
#include <tsc.h>

__BEGIN_SYS

class Timer_Common
{
protected:
    Timer_Common() {}

public:
    typedef TSC::Hertz Hertz;
    typedef TSC::Hertz Tick;
    typedef Handler::Function Handler;
    typedef int Channel;
    enum {
	SCHEDULER,
	ALARM,
	USER_FIRST,
	USER = USER_FIRST
    };
};

class Dummy_Timer: public Timer_Common
{
public:
    Dummy_Timer(const Hertz & frequency,
		const Handler * handler,
		const Channel & channel) {}
    
    int reset() { return 0; }
};

__END_SYS

#ifdef __TIMER_H
#include __TIMER_H
#endif

#endif
