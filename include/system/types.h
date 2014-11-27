// EPOS Internal Type Management System

typedef __SIZE_TYPE__ size_t;

#ifndef __types_h
#define __types_h

// Memory allocators
__BEGIN_API
enum System_Allocator { SYSTEM };
enum Scratchpad_Allocator { SCRATCHPAD };
__END_API

extern "C"
{
    void * malloc(size_t);
    void free(void *);
}

inline void * operator new(size_t s, void * a) { return a; }
inline void * operator new[](size_t s, void * a) { return a; }

void * operator new(size_t, const EPOS::System_Allocator &);
void * operator new[](size_t, const EPOS::System_Allocator &);

void * operator new(size_t, const EPOS::Scratchpad_Allocator &);
void * operator new[](size_t, const EPOS::Scratchpad_Allocator &);

// Utilities
__BEGIN_UTIL
class Bitmaps;
class CRC;
class ELF;
class Handler;
class Hashes;
class Heaps;
class Debug;
class Lists;
class Observers;
class Observeds;
class OStream;
class Queues;
class Random;
class Spin;
class SREC;
class Vectors;
__END_UTIL

__BEGIN_SYS

// System parts
class Build;
class Boot;
class Setup;
class Init;

// Architecture Hardware Mediators
class IA32;
class IA32_TSC;
class IA32_MMU;
class IA32_PMU;

// Machine Hardware Mediators
class PC;
class PC_PCI;
class PC_IC;
class PC_Timer;
class PC_RTC;
class PC_EEPROM;
class PC_Scratchpad;
class PC_UART;
class PC_Display;
class PC_Ethernet;
class PCNet32;
class C905;
class E100;

class Serial_Display;

// Abstractions
class System;
class Application;

class Thread;
class Active;
class Periodic_Thread;
class RT_Thread;
class Task;

template<typename> class Scheduler;
namespace Scheduling_Criteria
{
    class Priority;
    class FCFS;
    class RR;
    class RM;
    class DM;
    class EDF;
    class CPU_Affinity;
    class GEDF;
    class PEDF;
    class CEDF;
};

class Segment;
class Address_Space;

class Synchronizer;
class Mutex;
class Semaphore;
class Condition;

class Clock;
class Alarm;
class Chronometer;

template<typename NIC, typename Network, unsigned int HTYPE = 1>
class ARP;
class Network;
class IP;
class ICMP;
class UDP;
class TCP;
class DHCP;

// Framework
class Framework;
template<typename Component> class Handle;
template<typename Component> class Adapter;

// Aspects
template<typename Component> class Identified;
template<typename Component> class Shared;
template<typename Component> class Remote;

// System Components IDs
// The order in this enumeration defines many things in the system (e.g. init)
typedef unsigned int Type_Id;
enum 
{
    CPU_ID = 0,
    TSC_ID,
    MMU_ID,

    MACHINE_ID = 10,
    PCI_ID,
    IC_ID,
    TIMER_ID,
    RTC_ID,
    EEPROM_ID,
    SCRATCHPAD_ID,
    UART_ID,
    DISPLAY_ID,
    NIC_ID,

    THREAD_ID = 20,
    TASK_ID,
    ACTIVE_ID,

    ADDRESS_SPACE_ID,
    SEGMENT_ID,

    MUTEX_ID,
    SEMAPHORE_ID,
    CONDITION_ID,

    CLOCK_ID,
    ALARM_ID,
    CHRONOMETER_ID,

    IP_ID,
    ICMP_ID,
    UDP_ID,
    TCP_ID,
    DHCP_ID,

    UNKNOWN_TYPE_ID,
    LAST_TYPE_ID = UNKNOWN_TYPE_ID - 1
};

// Type IDs for system components
template<typename T> struct Type { static const Type_Id ID = UNKNOWN_TYPE_ID; };

template<> struct Type<IA32> { static const Type_Id ID = CPU_ID; };
template<> struct Type<IA32_TSC> { static const Type_Id ID = TSC_ID; };
template<> struct Type<IA32_MMU> { static const Type_Id ID = MMU_ID; };

template<> struct Type<PC> { static const Type_Id ID = MACHINE_ID; };
template<> struct Type<PC_IC> { static const Type_Id ID = IC_ID; };
template<> struct Type<PC_Timer> { static const Type_Id ID = TIMER_ID; };
template<> struct Type<PC_UART> { static const Type_Id ID = UART_ID; };
template<> struct Type<PC_RTC> { static const Type_Id ID = RTC_ID; };
template<> struct Type<PC_PCI> { static const Type_Id ID = PCI_ID; };
template<> struct Type<PC_Display> { static const Type_Id ID = DISPLAY_ID; };
template<> struct Type<PC_Scratchpad> { static const Type_Id ID = SCRATCHPAD_ID; };
template<> struct Type<PC_Ethernet> { static const Type_Id ID = NIC_ID; };

template<> struct Type<Thread> { static const Type_Id ID = THREAD_ID; };
template<> struct Type<Active> { static const Type_Id ID = ACTIVE_ID; };
template<> struct Type<Task> { static const Type_Id ID = TASK_ID; };
template<> struct Type<Address_Space> { static const Type_Id ID = ADDRESS_SPACE_ID; };
template<> struct Type<Segment> { static const Type_Id ID = SEGMENT_ID; };
template<> struct Type<IP> { static const Type_Id ID = IP_ID; };
template<> struct Type<ICMP> { static const Type_Id ID = ICMP_ID; };
template<> struct Type<UDP> { static const Type_Id ID = UDP_ID; };
template<> struct Type<TCP> { static const Type_Id ID = TCP_ID; };
template<> struct Type<DHCP> { static const Type_Id ID = DHCP_ID; };

__END_SYS

#endif
