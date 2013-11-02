#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

// Global Configuration
template <typename T>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = false;
};

template <> struct Traits<Build>
{
    enum {LIBRARY, BUILTIN};
    static const unsigned int MODE = LIBRARY;

    enum {IA32};
    static const unsigned int ARCH = IA32;

    enum {PC};
    static const unsigned int MACH = PC;

    static const unsigned int CPUS = 1;
    static const unsigned int NODES = 2;
};


// Utilities
template <> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = true;
    static const bool trace   = true;
};

template <> struct Traits<Lists>: public Traits<void>
{
    static const bool debugged = false;
};

template <> struct Traits<Spin>: public Traits<void>
{
    static const bool debugged = false;
};

template <> struct Traits<Heap>: public Traits<void>
{
    static const bool debugged = false;
};


// System Parts (mostly to fine control debbugin)
template <> struct Traits<Boot>: public Traits<void>
{
};

template <> struct Traits<Setup>: public Traits<void>
{
};

template <> struct Traits<Init>: public Traits<void>
{
};


// Common Mediators
template <> struct Traits<Serial_Display>: public Traits<void>
{
    static const bool enabled = true;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

__END_SYS

#include __ARCH_TRAITS_H
#include __HEADER_MACH(config)
#include __MACH_TRAITS_H

__BEGIN_SYS

template <> struct Traits<Application>: public Traits<void>
{
    static const unsigned int STACK_SIZE = 256 * 1024;
    static const unsigned int HEAP_SIZE = 16 * 1024 * 1024;
};

template <> struct Traits<System>: public Traits<void>
{
    static const unsigned int mode = Traits<Build>::MODE;
    static const bool multithread = true;
    static const bool multitask = false && (mode != Traits<Build>::LIBRARY);
    static const bool multicore = false && multithread;
    static const bool multiheap = (mode != Traits<Build>::LIBRARY) || Traits<Scratchpad>::enabled;

    enum {FOREVER = 0, SECOND = 1, MINUTE = 60, HOUR = 3600, DAY = 86400,
          WEEK = 604800, MONTH = 2592000, YEAR = 31536000};
    static const unsigned long LIFE_SPAN = 1 * HOUR; // in seconds

    static const bool reboot = true;

    static const unsigned int STACK_SIZE = 256 * 1024;
    static const unsigned int HEAP_SIZE = 128 * Traits<Application>::STACK_SIZE;
};


// Abstractions
template <> struct Traits<Task>: public Traits<void>
{
    static const bool enabled = Traits<System>::multitask;
};

template <> struct Traits<Thread>: public Traits<void>
{
    static const bool smp = Traits<System>::multicore;

    typedef Scheduling_Criteria::RR Criterion;
    static const unsigned int QUANTUM = 10000; // us

    static const bool trace_idle = false;
};

template <> struct Traits<Scheduler<Thread> >: public Traits<void>
{
    static const bool debugged = false;
};

template <> struct Traits<Periodic_Thread>: public Traits<void>
{
    static const bool simulate_capacity = false;
};

template <> struct Traits<Address_Space>: public Traits<void>
{
    static const bool enabled = Traits<System>::multiheap;
};

template <> struct Traits<Segment>: public Traits<void>
{
    static const bool enabled = Traits<System>::multiheap;
};

template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
};

template <> struct Traits<Synchronizer>: public Traits<void>
{
    static const bool enabled = Traits<System>::multithread;
};

template <> struct Traits<IP>: public Traits<void>
{
    static const bool debugged = true;

    static const unsigned long ADDRESS   = 0x0a000102;   // 10.0.1.2
    static const unsigned long NETMASK   = 0xffffff00;   // 255.255.255.0
    static const unsigned long BROADCAST = 0x0a0001ff;   // 10.0.1.255
    static const unsigned long GATEWAY   = 0x0a000101;   // 10.0.1.1

//    static const bool forwarding    = false;
//    static const bool fragmentation = false;
//    static const bool spawn_thread  = true;

    enum {STATIC, DHCP};
    static const unsigned int CONFIG = STATIC;

//    static const unsigned int  OPT_SIZE = 0; // options size in 32-bit words
    static const unsigned int TTL  = 0x20; // Time-to-live
//    static const unsigned int  MAX_FRAGMENTS = 1;
};

template <> struct Traits<UDP>: public Traits<void>
{
    static const bool debugged = true;

    static const bool checksum = false;
};

__END_SYS

#endif
