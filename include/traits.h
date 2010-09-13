// EPOS Configuration

#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = true;
    static const bool power_management = false;
};


// Utilities
template <> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = false;
    static const bool trace   = false;
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
};


// System parts
template <> struct Traits<Boot>: public Traits<void>
{
};

template <> struct Traits<Setup>: public Traits<void>
{
};

template <> struct Traits<Init>: public Traits<void>
{
};

template <> struct Traits<System>: public Traits<void>
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


// Abstractions
template <> struct Traits<Thread>: public Traits<void>
{
    typedef Scheduling_Criteria::Priority Criterion;
    static const bool smp = false;
    static const bool trace_idle = false;
    static const unsigned int QUANTUM = 10000; // us
};

template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool idle_waiting = true;
};

template <> struct Traits<Synchronizer>: public Traits<void>
{
};

template <> struct Traits<Network>: public Traits<void>
{
    static const unsigned int ELP = 0x8888;
    static const unsigned int ARP_TRIES = 3;
    static const unsigned int ARP_TIMEOUT = 1000000; // 1s
};

template <> struct Traits<IP>: public Traits<void>{
    static const unsigned int ADDRESS = 0xc0a80a01;   // 192.168.10.1
    static const unsigned int NETMASK = 0xffffff00;   // 255.255.255.0
    static const unsigned int BROADCAST = 0; // 0= Default Broadcast Address
};

__END_SYS

#ifdef __ARCH_TRAITS_H
#include __ARCH_TRAITS_H
#endif

#ifdef __MACH_TRAITS_H
#include __MACH_TRAITS_H
#endif

#endif
