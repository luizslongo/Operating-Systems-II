// EPOS-- Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <system/kmalloc.h>
#include <utility/queue.h>
#include <utility/handler.h>
#include <cpu.h>
#include <mmu.h>

__BEGIN_SYS

class Thread
{
protected:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;

    typedef Ordered_Queue<Thread, int, Traits<Thread>::smp> Queue;

    static const unsigned int STACK_SIZE 
    = Traits<Machine>::APPLICATION_STACK_SIZE;

    static const bool idle_waiting = Traits<Thread>::idle_waiting;
    static const bool active_scheduler = Traits<Thread>::active_scheduler;
    static const bool preemptive = Traits<Thread>::preemptive;
    static const bool smp = Traits<Thread>::smp;

    static const unsigned int QUANTUM = Traits<Thread>::QUANTUM;

public:
    typedef short State;
    enum  {
        RUNNING,
        READY,
        SUSPENDED,
	WAITING,
	FINISHING
    };

    typedef short Priority;
    enum {
	HIGH = 0,
	NORMAL = 15,
	LOW = 30,
	IDLE = 31
    };

public:
    // The int left on the stack between thread's arguments and its context
    // is due to the fact that the thread's function believes it's a normal
    // function that will be invoked with a call, which pushes the return
    // address on the stack
    Thread(int (* entry)(), 
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)

    {
	prevent_scheduling();
	
	_thread_count++;
	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit,
				   entry);

	init_thread(entry, stack_size, state, priority);
    }
    template<typename T1>
    Thread(int (* entry)(T1 a1), T1 a1,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_thread_count++;
	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit,
				   entry, a1);

	init_thread(entry, stack_size, state, priority);
    }
    template<typename T1, typename T2>
    Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_thread_count++;
	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit,
				   entry, a1, a2);

	init_thread(entry, stack_size, state, priority);
    }
    template<typename T1, typename T2, typename T3>
    Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_thread_count++;
	_stack = kmalloc(stack_size);
	_context = CPU::init_stack(_stack, stack_size, &implicit_exit,
				   entry, a1, a2, a3);

	init_thread(entry, stack_size, state, priority);
    }
    ~Thread() {
	db<Thread>(TRC) << "~Thread(this=" << this 
			<< ",state=" << _state
			<< ",priority=" << _link.rank()
			<< ",stack={b=" << _stack
			<< ",context={b=" << _context
			<< "," << *_context << "})\n";

	switch(_state) {
	case READY: _ready.remove(this); break;
	case SUSPENDED: _suspended.remove(this); break;
	case WAITING: _waiting->remove(this); break;
	}
	
	kfree(_stack);
    }

    volatile const State & state() const { return _state; }
    Priority priority() const { return _link.rank(); }
    void priority(const Priority & p) { _link.rank(p); }

    int join();
    void pass();
    void suspend();
    void resume();

    static Thread * volatile  & self() { return _running; }
    static void yield();
    static void sleep(Queue * q);
    static void wakeup(Queue * q);
    static void wakeup_all(Queue * q);
    static void exit(int status = 0);

    static void init();

private:
    void init_thread(Log_Addr entry, unsigned int stack_size, State state,
		     Priority priority) {
	db<Thread>(TRC) << "Thread(entry=" << (void *)entry 
			<< ",state=" << _state
			<< ",priority=" << _link.rank()
			<< ",stack={b=" << _stack
			<< ",s=" << stack_size
			<< "},context={b=" << _context
			<< "," << *_context << "}) => " << this << "\n";

	_state = state;
	_waiting = 0;
	_joining = 0;

	switch(state) {
	case RUNNING: break;
	case SUSPENDED: _suspended.insert(&_link); break;
	default: _ready.insert(&_link);
	}

	allow_scheduling();

 	if(preemptive)
 	    reschedule();
    }

    static Thread * volatile  & running() { return _running; }
    static void running(Thread * r) { _running = r; }

    static void prevent_scheduling() {
	if(active_scheduler) CPU::int_disable();
    }
    static void allow_scheduling() {
	if(active_scheduler) CPU::int_enable();
    }

    static void reschedule(); // this is the master alarm handler

    static void implicit_exit();

    static void switch_to(Thread * n);

    static int idle();

private:
    Log_Addr _stack;
    Context * volatile _context;
    volatile State _state;
    Queue * _waiting;
    Thread * volatile _joining;
    Queue::Element _link;

    static unsigned int _thread_count;
    static Thread * volatile _running;
    static Thread * _idle;
    static Queue _ready;
    static Queue _suspended;
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
