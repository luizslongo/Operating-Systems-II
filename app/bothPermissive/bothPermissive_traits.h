#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

// Build
template<> struct Traits<Build>: public Traits_Tokens
{
    // Basic configuration
    static const unsigned int SMOD = LIBRARY;
    static const unsigned int ARCHITECTURE = IA32;
    static const unsigned int MACHINE = PC;
    static const unsigned int MODEL = Legacy_PC;
    static const unsigned int CPUS = ((MODEL == Legacy_PC) || (MODEL == Raspberry_Pi3) || (MODEL == Realview_PBX) || (MODEL == Zynq) || (MODEL == SiFive_U)) ? 4 : 1;
    static const unsigned int NETWORKING = STANDALONE;
    static const unsigned int EXPECTED_SIMULATION_TIME = 60; // s (0 => not simulated)

    // Default flags
    static const bool enabled = true;
    static const bool debugged = false;
    static const bool trace = false;
    static const bool monitored = true;
    static const bool hysterically_debugged = false;
};

template<> struct Traits<EDF_Modified> : public Traits<Build> {
    static const bool ENABLE_STATISTICS = false;
    static const bool ENABLE_DEADLINE_PRINT = false;
};


// Utilities
template<> struct Traits<Debug>: public Traits<Build>
{
    static const bool error   = false;
    static const bool warning = false;
    static const bool info    = false;
    static const bool trace   = false;
};

template<> struct Traits<Lists>: public Traits<Build>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Spin>: public Traits<Build>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Heaps>: public Traits<Build>
{
    static const bool debugged = hysterically_debugged;
};

template<> struct Traits<Observers>: public Traits<Build>
{
    // Some observed objects are created before initializing the Display
    // Enabling debug may cause trouble in some Machines
    static const bool debugged = false;
};


// System Parts (mostly to fine control debugging)
template<> struct Traits<Boot>: public Traits<Build>
{
};

template<> struct Traits<Setup>: public Traits<Build>
{
};

template<> struct Traits<Init>: public Traits<Build>
{
};

template<> struct Traits<Framework>: public Traits<Build>
{
};

template<> struct Traits<Aspect>: public Traits<Build>
{
    static const bool debugged = hysterically_debugged;
};


__END_SYS

// Mediators
#include __ARCHITECTURE_TRAITS_H
#include __MACHINE_TRAITS_H

__BEGIN_SYS


// API Components
template<> struct Traits<Application>: public Traits<Build>
{
    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = Traits<Machine>::HEAP_SIZE;
    static const unsigned int MAX_THREADS = Traits<Machine>::MAX_THREADS;
};

template<> struct Traits<System>: public Traits<Build>
{
    static const bool multithread = (Traits<Application>::MAX_THREADS > 1) || (CPUS > 1);
    static const bool multicore = multithread && (CPUS > 1);
    static const bool multiheap = Traits<Scratchpad>::enabled;

    static const unsigned long LIFE_SPAN = 1 * YEAR; // s
    static const unsigned int DUTY_CYCLE = 1000000; // ppm

    static const bool reboot = true;

    static const unsigned int STACK_SIZE = Traits<Machine>::STACK_SIZE;
    static const unsigned int HEAP_SIZE = (Traits<Application>::MAX_THREADS + Traits<Build>::CPUS) * Traits<Application>::STACK_SIZE;
};

template<> struct Traits<Thread>: public Traits<Build>
{
    static const bool enabled = Traits<System>::multithread;
    static const bool smp = Traits<System>::multicore;
    static const bool trace_idle = hysterically_debugged;
    static const bool simulate_capacity = false;
    static const int priority_inversion_protocol = INHERITANCE;

    typedef IF<(CPUS > 1), PEDF_Modified, EDF_Modified>::Result Criterion;
    static const unsigned int QUANTUM = 10000; // us
};

template<> struct Traits<Scheduler<Thread>>: public Traits<Build>
{
    static const bool debugged = Traits<Thread>::trace_idle || hysterically_debugged;
};

template<> struct Traits<Synchronizer>: public Traits<Build>
{
    static const bool enabled = Traits<System>::multithread;
};

template<> struct Traits<Alarm>: public Traits<Build>
{
    static const bool visible = hysterically_debugged;
};

template<> struct Traits<Address_Space>: public Traits<Build> {};

template<> struct Traits<Segment>: public Traits<Build> {};

__END_SYS

#endif
