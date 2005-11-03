// EPOS-- Internal Type Management System

#ifndef __types_h
#define __types_h

inline void * operator new(unsigned int s, void * a) { return a; }

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

// Hardware Mediators - Sensors
class Mica2_Temperature_Sensor;
class Mica2_Light_Sensor;
class PC_Sensor;

// Hardware Mediators - NICs
class PC_NIC;

// Abstractions
class Thread;
class Task;
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

class Network;

class Photo_Sentient;
class Temperature_Sentient;


// System Components IDs
typedef unsigned int Type_Id;
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

    TEMPERATURE_SENSOR_ID,
    MICA2_TEMPERATURE_SENSOR_ID = TEMPERATURE_SENSOR_ID,

    LIGHT_SENSOR_ID,
    MICA2_LIGHT_SENSOR_ID = LIGHT_SENSOR_ID,

    NIC_ID,
    PC_NIC_ID = NIC_ID,

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

    NETWORK_ID,
    
    TEMPERATURE_SENTIENT_ID,
    PHOTO_SENTIENT_ID,

    UNKNOWN_TYPE,
    LAST_TYPE = UNKNOWN_TYPE - 1,
    ANY_TYPE = 0xffff
};

// System Component Types (type -> id)
template<class T>
struct Type
{
    enum { TYPE = UNKNOWN_TYPE }; 
};

template<> struct Type<IA32>
{ enum { TYPE = IA32_ID }; };
template<> struct Type<AVR8>
{ enum { TYPE = AVR8_ID }; };

template<> struct Type<IA32_TSC>
{ enum { TYPE = IA32_TSC_ID }; };
template<> struct Type<AVR8_TSC>
{ enum { TYPE = AVR8_TSC_ID }; };

template<> struct Type<IA32_MMU>
{ enum { TYPE = IA32_MMU_ID }; };
template<> struct Type<AVR8_MMU>
{ enum { TYPE = AVR8_MMU_ID }; };


template<> struct Type<PC>
{ enum { TYPE = PC_ID }; };
template<> struct Type<AVRMCU> 
{ enum { TYPE = AVRMCU_ID }; };
template<> struct Type<Mica2> 
{ enum { TYPE = MICA2_ID }; };

template<> struct Type<PC_IC>
{ enum { TYPE = PC_IC_ID }; };
template<> struct Type<AVRMCU_IC>
{ enum { TYPE = AVRMCU_IC_ID }; };
template<> struct Type<Mica2_IC> 
{ enum { TYPE = MICA2_IC_ID }; };

template<> struct Type<PC_Timer> 
{ enum { TYPE = PC_TIMER_ID }; };
template<> struct Type<AVRMCU_Timer>
{ enum { TYPE = AVRMCU_TIMER_ID }; };
template<> struct Type<Mica2_Timer> 
{ enum { TYPE = MICA2_TIMER_ID }; };

template<> struct Type<PC_RTC>
{ enum { TYPE = PC_RTC_ID }; };

template<> struct Type<PC_PCI>
{ enum { TYPE = PC_PCI_ID }; };

template<> struct Type<PC_Display>
{ enum { TYPE = PC_DISPLAY_ID }; };
template<> struct Type<AVRMCU_Display>
{ enum { TYPE = AVRMCU_DISPLAY_ID }; };
template<> struct Type<Mica2_Display>
{ enum { TYPE = MICA2_DISPLAY_ID }; };

template<> struct Type<Mica2_Temperature_Sensor> 
{ enum { TYPE = MICA2_TEMPERATURE_SENSOR_ID }; };

template<> struct Type<Mica2_Light_Sensor> 
{ enum { TYPE = MICA2_LIGHT_SENSOR_ID }; };

template<> struct Type<PC_NIC>
{ enum { TYPE = PC_NIC_ID }; };


template<> struct Type<Thread>
{ enum { TYPE = THREAD_ID }; };


template<> struct Type<Segment>
{ enum { TYPE = SEGMENT_ID }; };


template<> struct Type<Address_Space>
{ enum { TYPE = ADDRESS_SPACE_ID }; };


template<> struct Type<Mutex> 
{ enum { TYPE = MUTEX_ID }; };
template<> struct Type<Semaphore>
{ enum { TYPE = SEMAPHORE_ID }; };
template<> struct Type<Condition>
{ enum { TYPE = CONDITION_ID }; };


template<> struct Type<Clock> 
{ enum { TYPE = CLOCK_ID }; };
template<> struct Type<Alarm> 
{ enum { TYPE = ALARM_ID }; };
template<> struct Type<Chronometer> 
{ enum { TYPE = CHRONOMETER_ID }; };


template<> struct Type<Network>
{ enum { TYPE = NETWORK_ID }; };


template<> struct Type<Temperature_Sentient>
{ enum { TYPE = TEMPERATURE_SENTIENT_ID }; };
template<> struct Type<Photo_Sentient>
{ enum { TYPE = PHOTO_SENTIENT_ID }; };

__END_SYS

#endif
