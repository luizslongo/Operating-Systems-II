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
            NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
            LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = true;

    public:
        Priority(int p = NORMAL): _priority(p) {}

        operator const volatile int() const volatile { return _priority; }

        void update_at_job_release() {}
        void update_at_job_begin() {}
        void update_at_job_end() {}

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
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
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
            IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
        };

        static const bool timed = false;
        static const bool dynamic = false;
        static const bool preemptive = false;

    public:
        FCFS(int p = NORMAL) :
            Priority((p == IDLE) ? IDLE : TSC::time_stamp()) {}
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
        static const bool dynamic = false;
        static const bool preemptive = true;

    public:
        RM(int p): Priority(p), _deadline(0) {} // Aperiodic
        RM(const RTC::Microsecond & d): Priority(PERIODIC), _deadline(d) {}

    private:
        RTC::Microsecond _deadline;
    };

     // Deadline Monotonic
     class DM: public Priority
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
              APERIODIC = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
              NORMAL    = APERIODIC,
              IDLE      = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
          };

          static const bool timed = false;
          static const bool dynamic = true;
          static const bool preemptive = true;

      public:
          EDF(int p): Priority(p), _deadline(0) {} // Aperiodic
          EDF(const RTC::Microsecond & d): Priority(d), _deadline(d) {}

          void update_at_job_release();
          void update_at_job_begin() {}
          void update_at_job_end() {}

      private:
          RTC::Microsecond _deadline;

          static volatile RTC::Microsecond _last_release;
      };
}


// Scheduling_Queue
template <typename T>
class Scheduling_Queue: public Scheduling_List<T, typename T::Criterion>
{
private:
    typedef Scheduling_List<T, typename T::Criterion> Base;

public:
    typedef typename Base::Element Element;

public:
    Element * remove(Element * e) {
        // removing object instead of element forces a search and renders
        // removing inexistent objects harmless
        return Base::remove(e->object());
    }

    Element * choose() {
        return Base::choose();
    }

    Element * choose(Element * e) {
        return Base::choose(e->object());
    }
};


// Scheduler
// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling queue sorting criterion (viz, through
// operators <, >, and ==) and must also define a method "link" to export the
// list element pointing to the object being handled.
template <typename T>
class Scheduler: public Scheduling_Queue<T>
{
private:
    typedef Scheduling_Queue<T> Base;

public:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler() {}

    unsigned int schedulables() { return Base::size(); }

    T * volatile chosen() { 
        return const_cast<T * volatile>(Base::chosen()->object());
    }

    void insert(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::insert(" << obj << ")\n";

        Base::insert(obj->link());
    }

    T * remove(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::remove(" << obj << ")\n";

        return Base::remove(obj->link()) ? obj : 0;
    }

    void suspend(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::suspend(" << obj << ")\n";

        Base::remove(obj->link());
    }

    void resume(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::resume(" << obj << ")\n";
        Base::insert(obj->link());
    }

    T * choose() {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::choose() => ";

        T * obj = Base::choose()->object();
        db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }

    T * choose_another() {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::choose_another() => ";

        T * obj = Base::choose_another()->object();
        db<Scheduler>(TRC) << obj << "\n";
        return obj;
    }

    T * choose(T * obj) {
        db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
                           << "]::choose(" << obj;

        if(!Base::choose(obj->link()))
            obj = 0;
        db<Scheduler>(TRC) << obj << "\n";

        return obj;
    }
};

__END_SYS

#endif
