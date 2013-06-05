// EPOS Scheduler Abstraction Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <utility/list.h>
#include <cpu.h>
#include <machine.h>

#include "scheduler_verified.h"

__BEGIN_SYS

// All scheduling criteria, or disciplins, must define operator int() with 
// semantics of returning the desired order of a given object in the 
// scheduling list
namespace Scheduling_Criteria
{
    
    // Priority (static and dynamic)
    class Priority
    {
    public:
        enum {
            MAIN   = 0,
            HIGH   = 1,
            NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
            LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool preemptive = true;
        static const bool energy_aware = false;

        static const unsigned int QUEUES = 1;

    public:
        Priority(int p = NORMAL): _priority(p) {}

        operator const volatile int() const volatile { return _priority; }

    protected:
        volatile int _priority;
    };


    // Round-Robin
    class Round_Robin: public Priority
    {
    public:
        enum {
            MAIN   = 0,
            NORMAL = 1,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = true;
        static const bool preemptive = true;
        static const bool energy_aware = false;

    public:
        Round_Robin(int p = NORMAL): Priority(p) {}
    };
    
    
    // First-Come, First-Served (FIFO)
    class FCFS: public Priority
    {
    public:
        enum {
            MAIN   = 0,
            NORMAL = 1,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool preemptive = false;
        static const bool energy_aware = false;

    public:
        FCFS(int p = NORMAL)
            : Priority((p == IDLE) ? IDLE : TSC::time_stamp()) {}
    };
    
    
    // Rate Monotonic
    class RM: public Priority
    {
    public:
        enum {
            MAIN      = 0,
            PERIODIC  = 1,
            APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
            NORMAL    = APERIODIC,
            IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool preemptive = true;
        static const bool energy_aware = false;

    public:
        RM(int p): Priority(p), _deadline(0) {} // Aperiodic
        RM(const RTC::Microsecond & d): Priority(PERIODIC), _deadline(d) {}

    private:
        RTC::Microsecond _deadline;
    };
    
    
    // Energy-aware Rate Monotonic
    class EARM: public RM
    {
    public:
        enum {
            MAIN	= 0,
            PERIODIC	= 1,
            APERIODIC	= (unsigned(1) << (sizeof(int) * 8 - 2)) - 1,
            IDLE	= (unsigned(1) << (sizeof(int) * 8 - 2)),
            BEST_EFFORT	= (unsigned(1) << (sizeof(int) * 8 - 2)) + 1,
            NORMAL	= APERIODIC
        };

        static const bool timed = true;
        static const bool preemptive = true;
        static const bool energy_aware = true;

    public:
        EARM(int p): RM(p) {}
        EARM(const RTC::Microsecond & d): RM(d) {}
    };
    
    
    // Earliest Deadline First
    class EDF: public Priority
    {
    public:
        enum {
            MAIN      = 0,
            PERIODIC  = 1,
            APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
            NORMAL    = APERIODIC,
            IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool preemptive = true;
        static const bool energy_aware = false;

    public:
        EDF(int p): Priority(p), _deadline(0) {} // Aperiodic
        EDF(const RTC::Microsecond & d): Priority(d >> 8), _deadline(d) {}

    private:
        RTC::Microsecond _deadline;
    };
    
    
    // CPU Affinity
    class CPU_Affinity: public Priority
    {
    public:
        static const unsigned int QUEUES = Traits<Machine>::MAX_CPUS;
  
    public:
        CPU_Affinity(int p = NORMAL): Priority(p),
             _affinity(((p == IDLE) || (p == MAIN)) ? Machine::cpu_id()
                   : ++_next_cpu %= Machine::n_cpus()) {}

        CPU_Affinity(int p, int a): Priority(p), _affinity(a) {}

        const volatile int & queue() const volatile { return _affinity; }

        static int current() { return Machine::cpu_id(); }

    private:
        volatile int _affinity;

        static int _next_cpu;
    };


}


__END_SYS

#endif
