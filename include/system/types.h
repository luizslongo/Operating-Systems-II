#ifndef __types_h
#define __types_h

__BEGIN_SYS

// Utilities
class Debug;

// System parts
class Boot;
class Setup;
class Init;
class System;
class Framework;
class Heap;

// Hardware Mediators - CPUs
class IA32;
class AVR8;

// Hardware Mediators - Time-Stamp Counters
class IA32_TSC;
class AVR8_TSC;

// Hardware Mediators - Memory Management Units
class IA32_MMU;
class AVR8_MMU;

// Hardware Mediators - Machines
class PC;
class AVRMCU;
class Mica2;

// Hardware Mediators - Interrupt Controllers
class PC_IC;
class AVRMCU_IC;
class Mica2_IC;

// Hardware Mediators - Timers
class PC_Timer;
class AVRMCU_Timer;
class Mica2_Timer;

// Hardware Mediators - Real Time Clocks
class PC_RTC;
class AVRMCU_RTC;
class Mica2_RTC;

// Hardware Mediators - Busses
class PC_PCI;

// Hardware Mediators - UARTs
class PC_UART;
class AVRMCU_UART;
class Mica2_UART;

// Hardware Mediators - Displays
class PC_Display;
class AVRMCU_Display;
class Mica2_Display;

// Abstractions
class Thread;
class Active;

class Segment;
class Address_Space;

class Synchronizer;
class Mutex;
class Semaphore;
class Condition;

class Clock;
class Alarm;
class Chronometer;

class Task;


// System Components IDs
typedef unsigned short Type_Id;
enum 
{
    CPU_ID,
    IA32_ID = CPU_ID,
    AVR8_ID,

    TSC_ID,
    IA32_TSC_ID = TSC_ID,
    AVR8_TSC_ID,

    MMU_ID,
    IA32_MMU_ID = MMU_ID,
    AVR8_MMU_ID,

    MACHINE_ID,
    PC_ID = MACHINE_ID,
    AVRMCU_ID,
    MICA2_ID,

    IC_ID,
    PC_IC_ID = IC_ID,
    AVRMCU_IC_ID,
    MICA2_IC_ID,

    TIMER_ID,
    PC_TIMER_ID = TIMER_ID,
    AVRMCU_TIMER_ID,
    MICA2_TIMER_ID,

    RTC_ID,
    PC_RTC_ID = RTC_ID,

    PCI_ID,
    PC_PCI_ID = PCI_ID,

    DISPLAY_ID,
    PC_DISPLAY_ID = DISPLAY_ID,
    AVRMCU_DISPLAY_ID,
    MICA2_DISPLAY_ID,

    THREAD_ID,

    SEGMENT_ID,

    ADDRESS_SPACE_ID,

    SYNCHRONIZER_ID,
    MUTEX_ID = SYNCHRONIZER_ID,
    SEMAPHORE_ID,
    CONDITION_ID,

    TIMEPIECE_ID,
    CLOCK_ID = TIMEPIECE_ID,
    ALARM_ID,
    CHRONOMETER_ID,

    UNKNOWN_TYPE,
    LAST_TYPE = UNKNOWN_TYPE - 1,
    ANY_TYPE = 0xffff
};

// Type IDs for system objects
template<class T> struct Type { static const Type_Id TYPE = UNKNOWN_TYPE; };

template<> struct Type<IA32> { static const Type_Id TYPE = IA32_ID; };
template<> struct Type<AVR8> { static const Type_Id TYPE = AVR8_ID; };

template<> struct Type<IA32_TSC> { static const Type_Id TYPE = IA32_TSC_ID; };
template<> struct Type<AVR8_TSC> { static const Type_Id TYPE = AVR8_TSC_ID; };

template<> struct Type<IA32_MMU> { static const Type_Id TYPE = IA32_MMU_ID; };
template<> struct Type<AVR8_MMU> { static const Type_Id TYPE = AVR8_MMU_ID; };

template<> struct Type<PC> { static const Type_Id TYPE = PC_ID; };
template<> struct Type<AVRMCU> { static const Type_Id TYPE = AVRMCU_ID; };
template<> struct Type<Mica2> { static const Type_Id TYPE = MICA2_ID; };

template<> struct Type<PC_IC> { static const Type_Id TYPE = PC_IC_ID; };
template<> struct Type<AVRMCU_IC> { static const Type_Id TYPE = AVRMCU_IC_ID; };
template<> struct Type<Mica2_IC> { static const Type_Id TYPE = MICA2_IC_ID; };

template<> struct Type<PC_Timer> { static const Type_Id TYPE = PC_TIMER_ID; };
template<> struct Type<AVRMCU_Timer> { static const Type_Id TYPE = AVRMCU_TIMER_ID; };
template<> struct Type<Mica2_Timer> { static const Type_Id TYPE = MICA2_TIMER_ID; };

template<> struct Type<PC_RTC> { static const Type_Id TYPE = PC_RTC_ID; };

template<> struct Type<PC_PCI> { static const Type_Id TYPE = PC_PCI_ID; };

template<> struct Type<PC_Display> { static const Type_Id TYPE = PC_DISPLAY_ID; };
template<> struct Type<AVRMCU_Display> { static const Type_Id TYPE = AVRMCU_DISPLAY_ID; };
template<> struct Type<Mica2_Display> { static const Type_Id TYPE = MICA2_DISPLAY_ID; };

template<> struct Type<Thread> { static const Type_Id TYPE = THREAD_ID; };

template<> struct Type<Segment> { static const Type_Id TYPE = SEGMENT_ID; };

template<> struct Type<Address_Space> { static const Type_Id TYPE = ADDRESS_SPACE_ID; };

template<> struct Type<Mutex> { static const Type_Id TYPE = MUTEX_ID; };
template<> struct Type<Semaphore> { static const Type_Id TYPE = SEMAPHORE_ID; };
template<> struct Type<Condition> { static const Type_Id TYPE = CONDITION_ID; };

template<> struct Type<Clock> { static const Type_Id TYPE = CLOCK_ID; };
template<> struct Type<Alarm> { static const Type_Id TYPE = ALARM_ID; };
template<> struct Type<Chronometer> { static const Type_Id TYPE = CHRONOMETER_ID; };

__END_SYS

#endif
