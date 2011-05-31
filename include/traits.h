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
    static const bool priority_alloc = false;
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
    static const unsigned short ELP = 0x8888;
    static const unsigned int   ARP_TRIES = 3;
    static const unsigned int   ARP_TIMEOUT = 1000000; // 1s
};

template <> struct Traits<IP>: public Traits<void>{
    static const unsigned long ADDRESS = 0xc0a80a01;   // 192.168.10.1
    static const unsigned long NETMASK = 0xffffff00;   // 255.255.255.0
    static const unsigned long BROADCAST = 0;

    static const bool fragmentation = false;
    static const int MAX_FRAGMENTS = 1;

    static const bool dynamic      = false; // true=dhcp false=static
    static const bool spawn_thread = true;  // automatic creation of IP's worker thread
    
    static const unsigned int OPT_SIZE = 0; // options size in 32-bit words
    static const unsigned char DEF_TTL = 0x40; // time-to-live

};

template <> struct Traits<UDP> : public Traits<IP> {
    static const bool checksum = false;
};

template <> struct Traits<TCP> : public Traits<IP> {
    static const bool checksum = true;
};

template <> struct Traits<ICMP> : public Traits<IP> {
    static const bool echo_reply = true; 
};

template <> struct Traits<CMAC<Radio_Wrapper> >: public Traits<void>
{
    static const bool debugged = false;

    static const bool time_triggered = false;
    static const bool coordinator    = false;
    static const bool ack            = false;
    static const bool csma           = false;
    static const bool rts_cts        = false;

    static const unsigned long SLEEPING_PERIOD = 1000;  // ms
    static const unsigned long TIMEOUT         = 50;    // ms
    static const unsigned long BACKOFF         = 2;     // ms
    static const unsigned char ADDRESS         = 0x0001;
    static const unsigned int  MTU             = 118; 
};

template <> struct Traits<Neighboring>: public Traits<void>
{
    static const bool enabled = true;

    static const unsigned int MAX_NEIGHBORS = 3;
};

template <> struct Traits<Service>: public Traits<void>
{
    static const bool enabled = true;

    // Services types
    enum {
        ARP = 0, // Network service
        ADHOP    // Network service
    };

    static const unsigned int NETWORK_SERVICE = ARP;
};

__END_SYS

#ifdef __ARCH_TRAITS_H
#include __ARCH_TRAITS_H
#endif

#ifdef __MACH_TRAITS_H
#include __MACH_TRAITS_H
#endif

#endif
