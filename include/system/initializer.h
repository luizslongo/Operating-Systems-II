// EPOS-- Initializer Declarations

#ifndef __initializer_h
#define __initializer_h

#include <system/config.h>

// Threads are handled latter (they can activate the sched.)
#define __LAST_INIT (Initializer::Dispatcher *)-1
#define INIT_TABLE {\
    &CPU::init, \
    &TSC::init, \
    &MMU::init, \
    &Machine::init, \
    &PCI::init, \
    &IC::init, \
    &Timer::init, \
    &RTC::init, \
    &Display::init, \
    &Address_Space::init, \
    &Segment::init, \
    &Mutex::init, \
    &Semaphore::init, \
    &Condition::init, \
    &Alarm::init, \
    &Clock::init, \
    &Chronometer::init, \
    __LAST_INIT \
} 

__BEGIN_SYS

class Initializer
{
private:
    typedef int (Dispatcher) (System_Info * );
    typedef void (Function)(void);

public:
    Initializer(System_Info * si);

private:
    static Dispatcher * init_table[];
};

__END_SYS

#endif
