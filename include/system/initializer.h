// EPOS-- Initializer Declarations

#ifndef __initializer_h
#define __initializer_h

#include <system/config.h>

__BEGIN_SYS

// Dummy class for incomplete architectures and machines 
class Dummy
{
public:
    static int init(System_Info * si) { return 0; }
};


// Threads are handled latter (they can activate the sched.)
#define __LAST_INIT (Initializer::Dispatcher *)-1
#define INIT_TABLE {\
    &CPU::init, \
    &MMU::init, \
    &TSC::init, \
    &Machine::init, \
    &PCI::init, \
    &IC::init, \
    &Timer::init, \
    &RTC::init, \
    &UART::init, \
    &ADC::init, \
    &Temperature_Sensor::init, \
    &Light_Sensor::init, \
    &NIC::init, \
    &Display::init, \
    &Address_Space::init, \
    &Segment::init, \
    &Mutex::init, \
    &Semaphore::init, \
    &Condition::init, \
    &Alarm::init, \
    &Clock::init, \
    &Chronometer::init, \
    &Network::init, \
    &Task::init, \
    __LAST_INIT \
} 

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
