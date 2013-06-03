// EPOS Scheduler Abstraction Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <utility/list.h>
#include <cpu.h>
#include <machine.h>

__BEGIN_SYS

// All scheduling criteria, or disciplines, must define operator int() with
// the semantics of returning the desired order of a given object within the
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
            NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) - 3,
            LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = true;

        static const unsigned int QUEUES = 1;
        static const bool multihead = false;

    public:
        Priority(int p = NORMAL): _priority(p) {}

        operator const volatile int() const volatile { return _priority; }

        void update() {}

    protected:
        volatile int _priority;
    };

    // Round-Robin
    class RR: public Priority
    {
    public:
        enum {
            MAIN   = 0,
            NORMAL = 1,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        static const bool timed = true;
        static const bool dynamic = false;
        static const bool preemptive = true;

    public:
        RR(int p = NORMAL): Priority(p) {}
    };

    // First-Come, First-Served (FIFO)
    class FCFS: public Priority
    {
    public:
        enum {
            MAIN   = 0,
            NORMAL = 1,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = false;

    public:
        FCFS(int p = NORMAL); // Defined at Alarm
    };

    // Rate Monotonic
    class RM: public Priority
    {
    public:
        enum {
            MAIN      = 0,
            PERIODIC  = 1,
            APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
            NORMAL    = APERIODIC,
            IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
        };

        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = true;

    public:
        RM(int p): Priority(p) {} // Aperiodic
        RM(const RTC::Microsecond & d): Priority(d) {}
    };

     // Deadline Monotonic
     class DM: public Priority
     {
     public:
         enum {
             MAIN      = 0,
             PERIODIC  = 1,
             APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
             NORMAL    = APERIODIC,
             IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
         };

         static const bool timed = false;
         static const bool dynamic = false;
         static const bool preemptive = true;

     public:
         DM(int p): Priority(p), _deadline(0) {} // Aperiodic
         DM(const RTC::Microsecond & d): Priority(d), _deadline(d) {}

     private:
         RTC::Microsecond _deadline;
     };

      // Earliest Deadline First
      class EDF: public Priority
      {
      public:
          enum {
              MAIN      = 0,
              PERIODIC  = 1,
              APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
              NORMAL    = APERIODIC,
              IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
          };

          static const bool timed = false;
          static const bool dynamic = true;
          static const bool preemptive = true;

      public:
          EDF(int p): Priority(p), _deadline(0) {} // Aperiodic
          EDF(const RTC::Microsecond & d); // Defined at Alarm

          void update(); // Defined at Alarm

      private:
          RTC::Microsecond _deadline;
      };

      // CPU Affinity
      class CPU_Affinity: public Priority
      {
      public:
          enum {
              MAIN   = 0,
              HIGH   = 1,
              NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) - 3,
              LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) - 2,
              IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) - 1
          };

          static const bool timed = false;
          static const bool dynamic = false;
          static const bool preemptive = true;

          static const unsigned int QUEUES = Traits<Machine>::MAX_CPUS;

      public:
          CPU_Affinity(int p = NORMAL): Priority(p),
          _affinity(((p == IDLE) || (p == MAIN)) ? Machine::cpu_id() : ++_next_cpu %= Machine::n_cpus()) {}

          CPU_Affinity(int p, unsigned int a): Priority(p), _affinity(a) {}

          const volatile unsigned int & queue() const volatile { return _affinity; }

          static unsigned int current() { return Machine::cpu_id(); }

      private:
          volatile unsigned int _affinity;
          static volatile unsigned int _next_cpu;
      };
}


// Scheduling_Queue
template <typename T, unsigned int Q, bool multihead>
class Scheduling_Queue;

template <typename T>
class Scheduling_Queue<T, 1, false>: public Scheduling_List<T, typename T::Criterion> {};

template <typename T, unsigned int Q>
class Scheduling_Queue<T, Q, false>: public Scheduling_Multihead_List<T, typename T::Criterion> {};

template <typename T, unsigned int Q>
class Scheduling_Queue<T, Q, true>: public Scheduling_Multihead_List<T, typename T::Criterion> {};

// Scheduler
// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling queue sorting criterion (viz, through
// operators <, >, and ==) and must also define a method "link" to export the
// list element pointing to the object being handled.
template <typename T>
class Scheduler: public Scheduling_Queue<T, T::Criterion::QUEUES, T::Criterion::multihead>
{
private:
    typedef Scheduling_Queue<T, T::Criterion::QUEUES, T::Criterion::multihead> Base;

public:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler() {}

    unsigned int schedulables() { return Base::size(); }

    T * volatile chosen() { 
    	// If called before insert(), chosen will dereference a null pointer!
    	// For threads, we this won't happen (see Thread::init()).
    	// But if you are unsure about your new use of the scheduler,
    	// please, pay the price of the extra "if" bellow.
//    	return const_cast<T * volatile>((Base::chosen()) ? Base::chosen()->object() : 0);
    	return const_cast<T * volatile>(Base::chosen()->object());
    }

    void insert(T * obj) {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::insert(" << obj << ")\n";

        Base::insert(obj->link());
    }

    T * remove(T * obj) {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::remove(" << obj << ")\n";

        return Base::remove(obj->link()) ? obj : 0;
    }

    void suspend(T * obj) {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::suspend(" << obj << ")\n";

        Base::remove(obj->link());
    }

    void resume(T * obj) {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::resume(" << obj << ")\n";

        Base::insert(obj->link());
    }

    T * choose() {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::choose() => ";

        T * obj = Base::choose()->object();
        db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }

    T * choose_another() {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::choose_another() => ";

        T * obj = Base::choose_another()->object();
        db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }

    T * choose(T * obj) {
        db<Scheduler>(TRC) << "Scheduler<" << Criterion::current() << ">[chosen=" << chosen()
                           << "]::choose(" << obj;

        if(!Base::choose(obj->link()))
            obj = 0;
        db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }
};

__END_SYS

#endif
