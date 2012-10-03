// EPOS Thread Abstraction Declarations

#ifndef __thread_h
#define __thread_h

#include <system/kmalloc.h>
#include <utility/queue.h>
#include <cpu.h>
#include <machine.h>

__BEGIN_SYS

class Thread
{
    friend class Synchronizer_Common;

protected:
    static const bool active_scheduler = Traits<Thread>::active_scheduler;
    static const bool preemptive = Traits<Thread>::preemptive;
    static const bool energy_aware = Traits<Thread>::energy_aware;
    static const bool smp = Traits<Thread>::smp;
	static const unsigned int MAX_CPUS = Traits<Machine>::MAX_CPUS;

    static const unsigned int QUANTUM = Traits<Thread>::QUANTUM;
    static const unsigned int STACK_SIZE =
        Traits<Machine>::APPLICATION_STACK_SIZE;

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
    typedef short Priority;
    enum {
        HIGH = 0,
        NORMAL = 15,
        LOW = 31
    };

    // Thread Queue
    typedef Queue<Thread> ThreadQueue;

public:
    Thread(int (* entry)(), 
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry);

        common_constructor(entry, stack_size);
    }

    template<typename T1>
    Thread(int (* entry)(T1 a1), T1 a1,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry, 
                                   a1);

        common_constructor(entry, stack_size);
    }

    template<typename T1, typename T2>
    Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry,
                                   a1, a2);

        common_constructor(entry, stack_size);
    }

    template<typename T1, typename T2, typename T3>
    Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
           const State & state = READY,
           const Priority & priority = NORMAL,
           unsigned int stack_size = STACK_SIZE)
    : _state(state), _link(this)
    {
        lock();

        _stack = kmalloc(stack_size);
        _context = CPU::init_stack(_stack, stack_size, &implicit_exit, entry, 
                                   a1, a2, a3);

        common_constructor(entry, stack_size);
    }

    ~Thread();

    const volatile State & state() const { return _state; }

    volatile const Priority  & priority() const { return _priority; }

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

    static Thread* volatile running() { return _running; }

    ThreadQueue::Element* link() { return &_link; }

    static void lock() {
        CPU::int_disable();
        if(smp)
            _lock.acquire();
    }

    static void unlock() {
        if(smp)
            _lock.release();
        CPU::int_enable();
    }


    static void reschedule();

    static void time_slicer();

    static void implicit_exit();

    static void idle();

protected:
    Log_Addr _stack;
    Context * volatile _context;
    volatile State _state;
    ThreadQueue::Element _link;

    static Spin _lock;
    static Scheduler_Timer * _timer;

private:
    volatile Priority _priority;

private:
    static Thread * volatile _running;
    static ThreadQueue _ready;
    static ThreadQueue _suspended;
};


__END_SYS

#endif
