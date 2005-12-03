// EPOS-- Init Declarations

#ifndef __init_h
#define __init_h

#include <system/config.h>

__BEGIN_SYS

// Dummy class for incomplete architectures and machines 
class Dummy
{
public:
    static int init(System_Info * si) { return 0; }
};


// Threads are handled latter (they can activate the sched.)
#define __LAST_INIT (Init::Dispatcher *)-1
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

//    &NIC::init,							\

__END_SYS

#endif
