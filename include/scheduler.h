// EPOS Scheduler Abstraction Declarations

#ifndef __scheduler_h
#define __scheduler_h

#include <utility/list.h>
#include <cpu.h>
#include <machine.h>

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
	static const bool GLOBAL_SCHEDULER = false;

	static const int queue() { return 0; }

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

	// Global Earliest Deadline First
    class GEDF: public EDF
    {
    public:
    static const unsigned int QUEUES = 1;
    static const bool GLOBAL_SCHEDULER = true;
    
    public:     
    GEDF(int p = NORMAL): EDF(p) { } // Aperiodic
    GEDF(const RTC::Microsecond & d): EDF(d) { }
                
    static int current() { return Machine::cpu_id(); }
    };
    
    // Global Rate Monotonic
    class GRM: public RM
    {
    public:
    static const unsigned int QUEUES = 1;
    static const bool GLOBAL_SCHEDULER = true;
    
    public:     
    GRM(int p = NORMAL): RM(p) { } // Aperiodic
    GRM(const RTC::Microsecond & d): RM(d) { }
        
    static int current() { return Machine::cpu_id(); }
    };
};

// Scheduling_Queue
// specialization for GLOBAL SCHEDULER, one queue, Q ready tasks
template <typename T, unsigned int Q, bool GLOBAL>
class Scheduling_Queue
{
private:
    typedef typename T::Criterion Criterion;
    typedef Global_Scheduling_List<T, Criterion> Queue;
    
public:
    typedef typename Queue::Element Element;
public:
    Scheduling_Queue() { 
      for(unsigned int i = 0; i < Q; i++)
        _running_elem[i] = 0;
      _run_size = 0;
      _lowest_priority_cpu = 0;
    }

    unsigned int size() { return _ready.size(); }
    
    Element * head_in_queue(int q) {
        return _running_elem[q];
    }
    
    int get_lowest_priority_running_cpu() { 
      update_lowest_priority_cpu();
      return _lowest_priority_cpu;
    }
    
    Element * volatile & chosen() { 
    return _running_elem[Criterion::current()];
    }

    void insert(Element * e) {
      if(_run_size < Q) {
        register int cpu_id = Criterion::current();
        if(_running_elem[cpu_id] != 0) {
            if(_running_elem[cpu_id]->rank() > e->rank()) {
            _ready.insert(_running_elem[cpu_id]);
            _running_elem[cpu_id] = e;
            } else {
              _ready.insert(e);
            }
        } else {
          _running_elem[cpu_id] = e; 
          _run_size++;
        }
      } else {
        _ready.insert(e);
      }
    }

    Element * remove(Element * e) {
      // removing object instead of element forces a search and renders
      // removing inexistent objects harmless
      //kout << "remove() e = " << (void *) e << " rank = " << e->rank();
      register int cpu_id = Criterion::current();
      if(_running_elem[cpu_id] == e) {
        _running_elem[cpu_id] = _ready.head();
        _ready.remove_head(); 
      } else {
        e = _ready.remove(e);
      }
      return e;
    }

    Element * choose() { 
      register int cpu_id = Criterion::current();
      if(size() > 0 && _ready.head()->rank() != Criterion::IDLE) {
        Element *e = _ready.head(); 
        if(e->rank() < _running_elem[cpu_id]->rank()) {
          _ready.remove_head(); 
          _ready.insert(_running_elem[cpu_id]);
          _running_elem[cpu_id] = e;
        }
      }
      return _running_elem[cpu_id];
    }

    Element * choose_another() {
      register int cpu_id = Criterion::current();
      if(size() > 0) {
        Element *e;
        e = _ready.head();
        if(e->rank() < _running_elem[cpu_id]->rank()) {
          _ready.remove_head(); //remove_head()
          _ready.insert(_running_elem[cpu_id]);
          _running_elem[cpu_id] = e;
        }
      }
      return _running_elem[cpu_id];
    }

    Element * choose(Element * e) {
      register int cpu_id = Criterion::current();
      if(e != _running_elem[cpu_id]) {
        _ready.insert(_running_elem[cpu_id]);
        _ready.remove(e);
        _running_elem[cpu_id] = e;
      }
      return _running_elem[cpu_id];
    }

private:
    void update_lowest_priority_cpu(void) {
        for(unsigned int  i = 0; i < Q; i++) {
            if(_running_elem[i] == 0) {
              _lowest_priority_cpu = i;
              break;
            } else {
                if(_running_elem[i]->rank() > _running_elem[_lowest_priority_cpu]->rank())
                    _lowest_priority_cpu = i;
            }
        }
    }
private:  
    Element * volatile _running_elem[Q];
    unsigned int _run_size;
    int _lowest_priority_cpu;
    Queue _ready;
};

//specialization for multi-queue, not GLOBAL SCHEDULER
template <typename T, unsigned int Q>
class Scheduling_Queue<T, Q, false>
{
private:
    typedef typename T::Criterion Criterion;
    typedef Scheduling_List<T, Criterion> Queue;

public:
    typedef typename Queue::Element Element;

public:
    Scheduling_Queue() {}

    unsigned int size() { return _ready[Criterion::current()].size(); }

	Element * head_in_queue(int q) {
        return _ready[q].chosen();
    }
    
    int get_lowest_priority_running_cpu() { return 0; }

    Element * volatile & chosen() { 
	return _ready[Criterion::current()].chosen();
    }

    void insert(Element * e) {
	_ready[e->rank().queue()].insert(e); 
    }

    Element * remove(Element * e) {
	// removing object instead of element forces a search and renders
	// removing inexistent objects harmless
	return _ready[e->rank().queue()].remove(e->object());
    }

    Element * choose() {
	return _ready[Criterion::current()].choose();
    }

    Element * choose_another() {
	return _ready[Criterion::current()].choose_another();
    }

    Element * choose(Element * e) {
	return _ready[e->rank().queue()].choose(e->object());
    }

private:
    Queue _ready[Q];
};

// Specialization for single-queue
template <typename T>
class Scheduling_Queue<T, 1, false>: public Scheduling_List<T, typename T::Criterion>
{
private:
    typedef Scheduling_List<T, typename T::Criterion> Base;

public:
    typedef typename Base::Element Element;

public:
	Element * head_in_queue(int q) {
        return Base::chosen();
    }
  
    int get_lowest_priority_running_cpu() { return 0; }

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
class Scheduler: public Scheduling_Queue<T, T::Criterion::QUEUES, T::Criterion::GLOBAL_SCHEDULER>
{
private:
    typedef Scheduling_Queue<T, T::Criterion::QUEUES, T::Criterion::GLOBAL_SCHEDULER> Base;

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

	int get_lowest_priority_running_cpu() {
    return Base::get_lowest_priority_running_cpu();   
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
