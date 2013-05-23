// EPOS Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <system/kmalloc.h>
#include <utility/queue.h>
#include <utility/handler.h>
#include <cpu.h>
#include <machine.h>

__BEGIN_SYS

class Thread
{
    friend class Synchronizer_Common;
    friend class Alarm;

protected:
    static const bool preemptive = Traits<Thread>::preemptive;
    static const bool reboot = Traits<System>::reboot;

    static const unsigned int QUANTUM = Traits<Thread>::QUANTUM;
    static const unsigned int STACK_SIZE = Traits<Application>::STACK_SIZE;

    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::Context Context;

public:
    // Thread State
    enum State {
        READY,
        RUNNING,
        SUSPENDED,
        WAITING,
        FINISHING
    };

    // Thread Priority
    typedef int Priority;
    enum {
        MAIN   = 0,
        HIGH   = 1,
        NORMAL = (unsigned(1) << (sizeof(int) * 8 - 1)) -4,
        LOW    = (unsigned(1) << (sizeof(int) * 8 - 1)) -3,
        IDLE   = (unsigned(1) << (sizeof(int) * 8 - 1)) -2
    };

    // Thread Queue
    typedef Ordered_Queue<Thread, Priority> Queue;

public:
    Thread(int (* entry)(), 
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this, priority)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size,
                                   &implicit_exit, entry);

        common_constructor(entry, stack_size); // implicit unlock
    }

    template<typename T1>
    Thread(int (* entry)(T1 a1), T1 a1,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this, priority)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size,
                                   &implicit_exit, entry, a1);

        common_constructor(entry, stack_size); // implicit unlock()
    }

    template<typename T1, typename T2>
    Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this, priority)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size,
                                   &implicit_exit, entry, a1, a2);

        common_constructor(entry, stack_size); // implicit unlock()
    }

    template<typename T1, typename T2, typename T3>
    Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this, priority)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size,
                                   &implicit_exit, entry, a1, a2, a3);

        common_constructor(entry, stack_size); // implicit unlock()
    }

    ~Thread();

    const volatile State & state() const { return _state; }

    const volatile Priority  & priority() const { return _link.rank(); }
    void priority(const Priority & p);

    int join();
    void pass();
    void suspend();
    void resume();

    static Thread* self() { return running(); }
    static void yield();
    static void exit(int status = 0);

    static void init();

protected:
    void common_constructor(Log_Addr entry, unsigned int stack_size);

    static Thread * volatile running() { return _running; }

    static void lock() { CPU::int_disable(); }

    static void unlock() { CPU::int_enable(); }

    static void sleep(Queue * q);

    static void wakeup(Queue * q);

    static void wakeup_all(Queue * q);

    static void reschedule();

    static void implicit_exit();

    static void dispatch(Thread * prev, Thread * next) {
        if(prev != next) {
            // prev->_context->save(); // can be used to force an update
            db<Thread>(TRC) << "Thread::dispatch(prev=" << prev
                			<< ",next=" << next << ")\n";
            db<Thread>(INF) << "prev={" << prev << ","
			                << *prev->_context << "}\n";
            db<Thread>(INF) << "next={" << next << ","
                    		<< *next->_context << "}\n";

            CPU::switch_context(&prev->_context, next->_context);
        }

        CPU::int_enable();
    }

    static int idle();

protected:
    Log_Addr _stack;
    Context * volatile _context;
    volatile State _state;
    Queue * _waiting;
    Thread * volatile _joining;
    Queue::Element _link;

    static volatile unsigned int _thread_count;
    static Scheduler_Timer * _timer;

private:
    static Thread * volatile _running;
    static Queue _ready;
    static Queue _suspended;
};


// An event handler that triggers a thread (see handler.h)
class Thread_Handler : public Handler
{
public:
    Thread_Handler(Thread * h) : _handler(h) {}
    ~Thread_Handler() {}

    void operator()() { _handler->resume(); }

private:
    Thread * _handler;
};

__END_SYS

#endif
