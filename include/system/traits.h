#ifndef __system_traits_h
#define __system_traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool enabled = true;
    static const bool debugged = true;
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
    static const bool debugged = false;
};


// System Parts
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
    static const bool reboot = true;
};

template <> struct Traits<Application>: public Traits<void>
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
    static const bool multicore = false;
    static const bool preemptive = true;
    static const bool trace_idle = true;
    static const unsigned int QUANTUM = 10000; // us
};

template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
};

template <> struct Traits<Synchronizer>: public Traits<void>
{
};

__END_SYS

#endif
