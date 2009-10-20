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
	    NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
	    LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
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

    // CPU Affinity
    class CPU_Affinity: public Priority
    {
    public:
	CPU_Affinity(int p = NORMAL, int a = 0): Priority(p), _affinity(a) {}

	const volatile int & affinity() const volatile { return _affinity; }
	void affinity(int a) { _affinity = a; }

    protected:
	volatile int _affinity;
    };
};


// Objects subject to scheduling by Scheduler must define a "link" method 
// to access the list element pointing to the object being manipulated.
template <typename T, typename C>
class Scheduler
{
public:
    typedef Scheduling_List<T, C> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler() {}

    unsigned int schedulables() { return _ready.size(); }

    T * volatile chosen() { 
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

	// removing obj instead of obj->link() forces a search and renders
	// removing inexistent objects harmless
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
    Queue _ready;
//     Queue _suspended;
};

// Specialization for CPU_Affinity (SMP)
template <typename T>
class Scheduler<T, Scheduling_Criteria::CPU_Affinity>
{
public:
    typedef Scheduling_Criteria::CPU_Affinity Affinity;
    typedef Scheduling_List<T, Affinity> Queue;
    typedef typename Queue::Element Element;

public:
    Scheduler(): _next_cpu(0) {}

    unsigned int schedulables() { return _ready[Machine::cpu_id()].size(); }

    T * volatile chosen() { 
	return const_cast<T * volatile>
	    (_ready[Machine::cpu_id()].chosen()->object()); 
    }

    void insert(T * obj) {
	int queue;
	if((obj->link()->rank() == Affinity::IDLE) 
	   || (obj->link()->rank() == Affinity::MAIN))
	    queue = Machine::cpu_id();
	else {
	    _next_cpu = (_next_cpu + 1) % Machine::n_cpus();
	    queue = _next_cpu;
	}

	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::insert(" << obj << ")\n";

	obj->link()->rank(Affinity(obj->link()->rank(), queue));

	_ready[queue].insert(obj->link()); 
    }

    T * remove(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::remove(" << obj << ")\n";

	Element * e = _ready[obj->link()->rank().affinity()].remove(obj);

	return e ? obj : 0;
    }

    void suspend(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::suspend(" << obj << ")\n";

	_ready[obj->link()->rank().affinity()].remove(obj);
    }

    void resume(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() 
			   << "]::resume(" << obj << ")\n";

	_ready[obj->link()->rank().affinity()].insert(obj->link());
    }

    T * choose() {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::choose() => ";

	T * obj = _ready[Machine::cpu_id()].choose()->object();

	db<Scheduler>(TRC) << obj << "\n";
	
	return obj;
    }

    T * choose_another() {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen()
			   << "]::choose_another() => ";

	T * obj = _ready[Machine::cpu_id()].choose_another()->object();

	db<Scheduler>(TRC) << obj << "\n";

	return obj;
    }

    T * choose(T * obj) {
	db<Scheduler>(TRC) << "Scheduler[chosen=" << chosen() 
			   << "]::choose(" << obj;

	if(!_ready[obj->link()->rank().affinity()].choose(obj))
	    obj = 0;

	db<Scheduler>(TRC) << obj << "\n";
	
	return obj;
    }

private:
    int _next_cpu;
    Queue _ready[8];
};

__END_SYS

#endif
