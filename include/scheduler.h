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
	    HIGH = 0,
	    MAIN = 127,
	    NORMAL = 128,
	    LOW = 254,
	    IDLE = 255
	};

    public:
	Priority(int p = NORMAL): _priority(p) {}

	operator const volatile int() const volatile { return _priority; }

    protected:
	volatile int _priority;
    };

    // Priority
    class EDF
    {
    protected:
	typedef RTC::Microsecond Microsecond;

    public:
	enum {
	    MAIN = 0,
	    NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -2,
	    IDLE = (unsigned(1) << (sizeof(int) * 8 - 1)) -1
	};

    public:
	EDF(int p): _deadline(0), _priority(p) {}
	EDF(const Microsecond & d): _deadline(d), _priority(d >> 8) {}

	operator const volatile int() const volatile { return _priority; }

    protected:
	RTC::Microsecond _deadline;
	int _priority;
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
    typedef typename T::Criterion Rank_Type;

    static const bool smp = Traits<Thread>::smp;

    typedef Scheduling_Queue<T, Rank_Type, smp> Queue;

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
	return _ready.remove(obj) ? obj : 0;
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
	db<Scheduler>(TRC) << ") => " << obj << "\n";
	return obj;
    }

private:
    Scheduling_Queue<Object_Type, Rank_Type, smp> _ready;
//     Queue<Object_Type, smp, Element> _suspended;
};

__END_SYS

#endif
