// EPOS-- Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <system/kmalloc.h>
#include <utility/queue.h>
#include <utility/handler.h>
#include <cpu.h>
#include <scheduler.h>

__BEGIN_SYS

class Thread
{
    friend class Scheduler<Thread>;

protected:
    static const unsigned int STACK_SIZE 
    = Traits<Machine>::APPLICATION_STACK_SIZE;

    static const bool idle_waiting = Traits<Thread>::idle_waiting;
    static const bool active_scheduler = Traits<Thread>::active_scheduler;
    static const bool preemptive = Traits<Thread>::preemptive;
    static const bool smp = Traits<Thread>::smp;

    static const unsigned int QUANTUM = Traits<Thread>::QUANTUM;

    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;

public:
    // Thread State
    enum State {
	BEGINNING,
	READY,
	RUNNING,
	SUSPENDED,
	WAITING,
	FINISHING
    };

    // Thread Priority
    typedef Scheduling_Criteria::Priority Priority;
    
    // Thread Scheduling Criterion
    typedef Traits<Thread>::Criterion Criterion;
    enum {
	NORMAL = Criterion::NORMAL,
	MAIN = Criterion::MAIN,
	IDLE = Criterion::IDLE
    };

    typedef
    Ordered_Queue<Thread, Criterion, smp, Scheduler<Thread>::Element> Queue;

public:
    Thread(int (* entry)(), 
	   const State & state = READY,
	   const Criterion & criterion = NORMAL,
	   unsigned int stack_size = STACK_SIZE)
	: _state(state), _waiting(0), _joining(0), _link(this, criterion)

    {
	prevent_scheduling();
	
	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry);

	common_constructor(entry, stack_size);
    }
    template<typename T1>
    Thread(int (* entry)(T1 a1), T1 a1,
	   const State & state = READY,
	   const Criterion & criterion = NORMAL,
	   unsigned int stack_size = STACK_SIZE)
	: _state(state), _waiting(0), _joining(0), _link(this, criterion)
    {
	prevent_scheduling();

	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry, 
				   a1);

	common_constructor(entry, stack_size);
    }
    template<typename T1, typename T2>
    Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2,
	   const State & state = READY,
	   const Criterion & criterion = NORMAL,
	   unsigned int stack_size = STACK_SIZE)
	: _state(state), _waiting(0), _joining(0), _link(this, criterion)
    {
	prevent_scheduling();

	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry,
				   a1, a2);

	common_constructor(entry, stack_size);
    }
    template<typename T1, typename T2, typename T3>
    Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
	   const State & state = READY,
	   const Criterion & criterion = NORMAL,
	   unsigned int stack_size = STACK_SIZE)
	: _state(state), _waiting(0), _joining(0), _link(this, criterion)
    {
	prevent_scheduling();

	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry, 
				   a1, a2, a3);

	common_constructor(entry, stack_size);
    }
    ~Thread();

    const volatile State & state() const { return _state; }
    const volatile Criterion & criterion() const { return _link.rank(); }

    Priority  priority() const { return int(_link.rank()); }
    void priority(const Priority & p);    

    int join();
    void pass();
    void suspend();
    void resume();

    static Thread * self() { return running(); }
    static void yield();
    static void sleep(Queue * q);
    static void wakeup(Queue * q);
    static void wakeup_all(Queue * q);
    static void exit(int status = 0);

    static void init();

protected:
    void common_constructor(Log_Addr entry, unsigned int stack_size);

    static Thread * volatile running() { return _scheduler.chosen(); }

    Queue::Element * link() { return &_link; }

    static void prevent_scheduling() {
	if(active_scheduler)
	    CPU::int_disable();
    }
    static void allow_scheduling() {
	if(active_scheduler)
	    CPU::int_enable();
    }

    static void reschedule();
    static void time_reschedule(); // this is the master alarm handler

    static void implicit_exit();

    static void switch_threads(Thread * prev, Thread * next);

    static int idle();

protected:
    Log_Addr _stack;
    Context * volatile _context;
    volatile State _state; 
    Queue * _waiting;
    Thread * volatile _joining;
    Queue::Element _link;

    static unsigned int _thread_count;
    static Scheduler<Thread> _scheduler;
};

// A thread event handler (see handler.h)
class Handler_Thread : public Handler
{
public:
    Handler_Thread(Thread * h) : _handler(h) {}
    ~Handler_Thread() {}

    void operator()() { _handler->resume(); }
	
private:
    Thread * _handler;
};

__END_SYS

#endif
