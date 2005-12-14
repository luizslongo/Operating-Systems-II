// EPOS-- Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <utility/queue.h>
#include <utility/malloc.h>
#include <utility/handler.h>
#include <cpu.h>
#include <mmu.h>

__BEGIN_SYS

class Thread
{
protected:
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;

    typedef Ordered_Queue<Thread, Traits<Thread>::smp> Queue;

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

	_stack = malloc(stack_size);
	_context = new (_stack + stack_size 
			- sizeof(int) - sizeof(Context)) Context(entry);

	header(entry, stack_size);

	Log_Addr sp = _stack + stack_size;

	body(state, priority, sp);
    }
    template<class T1>
    Thread(int (* entry)(T1 a1), T1 a1,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_stack = malloc(stack_size);
	_context = new (_stack + stack_size
			- sizeof(T1)
			- sizeof(int) - sizeof(Context)) Context(entry);

	header(entry, stack_size);

	Log_Addr sp = _stack + stack_size;
	sp -= sizeof(T1); *static_cast<T1 *>(sp) = a1;

	body(state, priority, sp);
    }
    template<class T1, class T2>
    Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_stack = malloc(stack_size);
	_context = new (_stack + stack_size 
			- sizeof(T2) - sizeof(T1)
			- sizeof(int) - sizeof(Context)) Context(entry);

	header(entry, stack_size);

	Log_Addr sp = _stack + stack_size;
	sp -= sizeof(T2); *static_cast<T2 *>(sp) = a2;
	sp -= sizeof(T1); *static_cast<T1 *>(sp) = a1;

	body(state, priority, sp);
    }
    template<class T1, class T2, class T3>
    Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
	   const State & state = READY,
	   const Priority & priority = NORMAL,
	   unsigned int stack_size = STACK_SIZE) : _link(this, priority)
    {
	prevent_scheduling();

	_stack = malloc(stack_size);
	_context = new (_stack + stack_size 
			- sizeof(T3) - sizeof(T2) - sizeof(T1)
			- sizeof(int) - sizeof(Context)) Context(entry);

	header(entry, stack_size);

	Log_Addr sp = _stack + stack_size;
	sp -= sizeof(T3); *static_cast<T3 *>(sp) = a3;
	sp -= sizeof(T2); *static_cast<T2 *>(sp) = a2;
	sp -= sizeof(T1); *static_cast<T1 *>(sp) = a1;

	body(state, priority, sp);
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
	
	free(_stack);
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

    static int init(System_Info * si);

private:
    void header(Log_Addr entry, unsigned int stack_size) {
	db<Thread>(TRC) << "Thread(entry=" << (void *)entry 
			<< ",state=" << _state
			<< ",priority=" << _link.rank()
			<< ",stack={b=" << _stack
			<< ",s=" << stack_size
			<< "},context={b=" << _context
			<< "," << *_context << "}) => " << this << "\n";
    }
    void body(State state, Priority priority, Log_Addr sp) {
	_state = state;
	_waiting = 0;
	_joining = 0;

	sp -= sizeof(int); 
	*static_cast<unsigned int *>(sp) = 
	    reinterpret_cast<unsigned int>(&implicit_exit);

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

    void operator()() { _handler->resume(); }
	
private:
    Thread * _handler;
};

__END_SYS

#endif
