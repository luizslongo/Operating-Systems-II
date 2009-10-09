// EPOS-- Scheduler Abstraction Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <utility/queue.h>
#include <rtc.h>

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
	    NORMAL =  (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
	    LOW    =  (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
	    IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
	};

	static const bool timed = false;
	static const bool preemptive = true;
	static const bool energy_aware = false;

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

	static const bool timed = true;
	static const bool preemptive = true;
	static const bool energy_aware = false;

    public:
	RM(int p): Priority(p), _deadline(0) {} // Aperiodic
	RM(const RTC::Microsecond & d): Priority(PERIODIC), _deadline(d) {}

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
	static const bool preemptive = true;
	static const bool energy_aware = false;

    public:
	EDF(int p): Priority(p), _deadline(0) {} // Aperiodic
	EDF(const RTC::Microsecond & d): Priority(d >> 8), _deadline(d) {}

    private:
	RTC::Microsecond _deadline;
    };
};

// Objects subject to scheduling by Scheduler must declare a type "Criterion"
// that will be used as the scheduling criterion (viz, through operators <, >,
// and ==) and must also define a method "link" to access the list element
// pointing to the object.
template <typename T>
class Scheduler
{
protected:
    typedef typename T::Criterion Policy;
    typedef typename T::Criterion Rank_Type;
    typedef Scheduling_Queue<T, Rank_Type> Queue;

    static const bool timed = Policy::timed;
    static const bool preemptive = Policy::preemptive;
    static const bool energy_aware = Policy::energy_aware;

public:
    typedef T Object_Type;
    typedef typename Queue::Element Element;
 
public:
    Scheduler() {}

    unsigned int schedulables() { return _ready.size(); }

    T * volatile  chosen() { 
	return const_cast<T * volatile>(_ready.chosen()->object()); 
    }

    void insert(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::insert(" << obj << ")\n";

	_ready.insert(obj->link()); 
    }

    T * remove(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::remove(" << obj << ")\n";

	// removing obj instead of obj->link() forces a search and makes
	// the removal of inexistent objects harmless
	Element * e = _ready.remove(obj);

	return e ? obj : 0;
    }

    void suspend(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::suspend(" << obj << ")\n";

	_ready.remove(obj);
// 	_suspend.insert(obj->link());
    }

    void resume(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() 
			   << "]::resume(" << obj << ")\n";

// 	_suspended.remove(obj->link());
	_ready.insert(obj->link());
    }

    T * choose() {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::choose() => ";

	T * obj = _ready.choose()->object();

	db<Scheduler>(TRC) << obj << "\n";
	
	return obj;
    }

    T * choose_another() {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::choose_another() => ";

	T * obj = _ready.choose_another()->object();

	db<Scheduler>(TRC) << obj << "\n";

	return obj;
    }

    T * choose(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() 
			   << "]::choose(" << obj;

	if(!_ready.choose(obj))
	    obj = 0;

	db<Scheduler>(TRC) << obj << "\n";
	
	return obj;
    }

private:
    Scheduling_Queue<Object_Type, Rank_Type> _ready;
//     Queue<Object_Type, Element> _suspended;
};

__END_SYS

#endif
