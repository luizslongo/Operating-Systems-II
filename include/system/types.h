#ifndef __types_h
#define __types_h

__BEGIN_SYS

// System Components
// System parts
class Boot;
class Setup;
class Init;
class System;
class Framework;
class Heap;

// Hardware Mediators - CPU
class IA32;
class PPC32;
class ARM;
class LANai;
class H8;
class AVR8;
typedef IA32 CPU;

// Hardware Mediators - Time-Stamp Counters
class IA32_TSC;
class PPC32_TSC;
class ARM_TSC;
class LANai_TSC;
class H8_TSC;
class AVR8_TSC;
typedef IA32_TSC TSC;

// Hardware Mediators - Memory Management Units
class IA32_MMU;
class PPC32_MMU;
class ARM_MMU;
class LANai_MMU;
class H8_MMU;
class AVR8_MMU;
typedef IA32_MMU MMU;

// Hardware Mediators - Machines
class PC;
class Khomp;
class iPAQ;
class Myrinet;
class RCX;
class AVRMCU;
typedef PC Machine;

// Hardware Mediators - Interrupt Controllers
class PC_IC;
class Khomp_IC;
class iPAQ_IC;
class Myrinet_IC;
class RCX_IC;
class AVRMCU_IC;
typedef PC_IC IC;

// Hardware Mediators - Timers
class PC_Timer;
class Khomp_Timer;
class iPAQ_Timer;
class Myrinet_Timer;
class RCX_Timer;
class AVRMCU_Timer;
typedef PC_Timer Timer;

// Hardware Mediators - Real Time Clocks
class PC_RTC;
class Khomp_RTC;
class iPAQ_RTC;
class AVRMCU_RTC;
typedef PC_RTC RTC;

// Hardware Mediators - Busses
class PC_PCI;
class Khomp_PCI;
typedef PC_PCI PCI;

// Hardware Mediators - UARTs
//class PC_UART;
//class AVRMCU_UART;
//typedef AVRMCU_UART UART;

// Hardware Mediators - ADCs
//class AVRMCU_ADC;
//typedef AVRMCU_ADC ADC;

// Hardware Mediators - Displays
class PC_Display;
class Khomp_Display;
class iPAQ_Display;
class Myrinet_Display;
class RCX_Display;
class AVRMCU_Display;
typedef PC_Display Display;

// Abstractions
class Thread;
class Segment;
class Address_Space;

class Mutex;
class Semaphore;
class Condition;

class Clock;
class Alarm;
class Chronometer;


// System Components IDs
typedef unsigned short Type_Id;
enum 
{
    CPU_ID,
    IA32_ID = CPU_ID,
    PPC32_ID,
    ARM_ID,
    LANAI_ID,
    H8_ID,
    AVR8_ID,

    TSC_ID,
    IA32_TSC_ID = TSC_ID,
    PPC32_TSC_ID,
    ARM_TSC_ID,
    LANAI_TSC_ID,
    H8_TSC_ID,
    AVR8_TSC_ID,

    MMU_ID,
    IA32_MMU_ID = MMU_ID,
    PPC32_MMU_ID,
    ARM_MMU_ID,
    LANAI_MMU_ID,
    H8_MMU_ID,
    AVR8_MMU_ID,

    MACHINE_ID,
    PC_ID = MACHINE_ID,
    KHOMP_ID,
    IPAQ_ID,
    MYRINET_ID,
    RCX_ID,
    AVRMCU_ID,

    IC_ID,
    PC_IC_ID = IC_ID,
    KHOMP_IC_ID,
    IPAQ_IC_ID,
    MYRINET_IC_ID,
    RCX_IC_ID,
    AVRMCU_IC_ID,

    TIMER_ID,
    PC_TIMER_ID = TIMER_ID,
    KHOMP_TIMER_ID,
    IPAQ_TIMER_ID,
    MYRINET_TIMER_ID,
    RCX_TIMER_ID,
    AVRMCU_TIMER_ID,

    RTC_ID,
    PC_RTC_ID = RTC_ID,
    KHOMP_RTC_ID,
    IPAQ_RTC_ID,

    PCI_ID,
    PC_PCI_ID = PCI_ID,
    KHOMP_PCI_ID,

    DISPLAY_ID,
    PC_DISPLAY_ID = DISPLAY_ID,
    KHOMP_DISPLAY_ID,
    IPAQ_DISPLAY_ID,
    MYRINET_DISPLAY_ID,
    RCX_DISPLAY_ID,
    AVRMCU_DISPLAY_ID,

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
template<> struct Type<PPC32> { static const Type_Id TYPE = PPC32_ID; };
template<> struct Type<ARM> { static const Type_Id TYPE = ARM_ID; };
template<> struct Type<LANai> { static const Type_Id TYPE = LANAI_ID; };
template<> struct Type<H8> { static const Type_Id TYPE = H8_ID; };
template<> struct Type<AVR8> { static const Type_Id TYPE = AVR8_ID; };

template<> struct Type<IA32_TSC> { static const Type_Id TYPE = IA32_TSC_ID; };
template<> struct Type<PPC32_TSC> { static const Type_Id TYPE = PPC32_TSC_ID; };
template<> struct Type<ARM_TSC> { static const Type_Id TYPE = ARM_TSC_ID; };
template<> struct Type<LANai_TSC> { static const Type_Id TYPE = LANAI_TSC_ID; };
template<> struct Type<H8_TSC> { static const Type_Id TYPE = H8_TSC_ID; };
template<> struct Type<AVR8_TSC> { static const Type_Id TYPE = AVR8_TSC_ID; };

template<> struct Type<IA32_MMU> { static const Type_Id TYPE = IA32_MMU_ID; };
template<> struct Type<PPC32_MMU> { static const Type_Id TYPE = PPC32_MMU_ID; };
template<> struct Type<ARM_MMU> { static const Type_Id TYPE = ARM_MMU_ID; };
template<> struct Type<LANai_MMU> { static const Type_Id TYPE = LANAI_MMU_ID; };
template<> struct Type<H8_MMU> { static const Type_Id TYPE = H8_MMU_ID; };
template<> struct Type<AVR8_MMU> { static const Type_Id TYPE = AVR8_MMU_ID; };

template<> struct Type<PC> { static const Type_Id TYPE = PC_ID; };
template<> struct Type<Khomp> { static const Type_Id TYPE = KHOMP_ID; };
template<> struct Type<iPAQ> { static const Type_Id TYPE = IPAQ_ID; };
template<> struct Type<Myrinet> { static const Type_Id TYPE = MYRINET_ID; };
template<> struct Type<RCX> { static const Type_Id TYPE = RCX_ID; };
template<> struct Type<AVRMCU> { static const Type_Id TYPE = AVRMCU_ID; };

template<> struct Type<PC_IC> { static const Type_Id TYPE = PC_IC_ID; };
template<> struct Type<Khomp_IC> { static const Type_Id TYPE = KHOMP_IC_ID; };
template<> struct Type<iPAQ_IC> { static const Type_Id TYPE = IPAQ_IC_ID; };
template<> struct Type<Myrinet_IC> { static const Type_Id TYPE = MYRINET_IC_ID; };
template<> struct Type<RCX_IC> { static const Type_Id TYPE = RCX_IC_ID; };
template<> struct Type<AVRMCU_IC> { static const Type_Id TYPE = AVRMCU_IC_ID; };

template<> struct Type<PC_Timer> { static const Type_Id TYPE = PC_TIMER_ID; };
template<> struct Type<Khomp_Timer> { static const Type_Id TYPE = KHOMP_TIMER_ID; };
template<> struct Type<iPAQ_Timer> { static const Type_Id TYPE = IPAQ_TIMER_ID; };
template<> struct Type<Myrinet_Timer> { static const Type_Id TYPE = MYRINET_TIMER_ID; };
template<> struct Type<RCX_Timer> { static const Type_Id TYPE = RCX_TIMER_ID; };
template<> struct Type<AVRMCU_Timer> { static const Type_Id TYPE = AVRMCU_TIMER_ID; };

template<> struct Type<PC_RTC> { static const Type_Id TYPE = PC_RTC_ID; };
template<> struct Type<Khomp_RTC> { static const Type_Id TYPE = KHOMP_RTC_ID; };
template<> struct Type<iPAQ_RTC> { static const Type_Id TYPE = IPAQ_RTC_ID; };

template<> struct Type<PC_PCI> { static const Type_Id TYPE = PC_PCI_ID; };
template<> struct Type<Khomp_PCI> { static const Type_Id TYPE = KHOMP_PCI_ID; };

//template<> struct Type<PC_UART> { static const Type_Id TYPE = PC_UART_ID; };
//template<> struct Type<AVRMCU_UART> { static const Type_Id TYPE = AVRMCU_UART_ID; };

//template<> struct Type<AVRMCU_ADC> { static const Type_Id TYPE = AVRMCU_ADC_ID; };

template<> struct Type<PC_Display> { static const Type_Id TYPE = PC_DISPLAY_ID; };
template<> struct Type<Khomp_Display> { static const Type_Id TYPE = KHOMP_DISPLAY_ID; };
template<> struct Type<iPAQ_Display> { static const Type_Id TYPE = IPAQ_DISPLAY_ID; };
template<> struct Type<Myrinet_Display> { static const Type_Id TYPE = MYRINET_DISPLAY_ID; };
template<> struct Type<RCX_Display> { static const Type_Id TYPE = RCX_DISPLAY_ID; };
template<> struct Type<AVRMCU_Display> { static const Type_Id TYPE = AVRMCU_DISPLAY_ID; };

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
