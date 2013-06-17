// EPOS Periodic Thread Abstraction Declarations

// Periodic threads are achieved by programming an alarm handler to invoke
// p() on a control semaphore after each job (i.e. task activation). Base
// threads are created in BEGINNING state, so the scheduler won't dispatch
// them before the associate alarm and semaphore are created. The first job
// is dispatched by resume() (thus the _state = SUSPENDED statement)

#ifndef __periodic_thread_h
#define __periodic_thread_h

#include <utility/handler.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

// Aperiodic Thread
typedef Thread Aperiodic_Thread;

// Periodic Thread
class Periodic_Thread: public Thread
{
protected:
    // Alarm Handler for periodic threads under static scheduling policies
    class Static_Handler: public Semaphore_Handler
    {
    public:
        Static_Handler(Semaphore * s, Periodic_Thread * t): Semaphore_Handler(s) {}
        ~Static_Handler() {}
    };

    // Alarm Handler for periodic threads under dynamic scheduling policies
    class Dynamic_Handler: public Semaphore_Handler
    {
    public:
        Dynamic_Handler(Semaphore * s, Periodic_Thread * t): Semaphore_Handler(s), _thread(t) {}
        ~Dynamic_Handler() {}

        void operator()() {
            _thread->criterion().update();

            Semaphore_Handler::operator()();
        }

    private:
        Periodic_Thread * _thread;
    };

    typedef IF<Criterion::dynamic, Dynamic_Handler, Static_Handler>::Result Handler;

public:
    typedef RTC::Microsecond Microsecond;

    enum { INFINITE = RTC::INFINITE };

public:
    Periodic_Thread(int (* entry)(), const Microsecond & period, int times = INFINITE,
                    const State & state = READY, const Criterion & criterion = NORMAL, unsigned int stack_size = STACK_SIZE)
    : Thread(entry, SUSPENDED, (criterion != NORMAL) ? criterion : Criterion(period), stack_size),
      _semaphore(0), _handler(&_semaphore, this), _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1>
    Periodic_Thread(int (* entry)(T1 a1), T1 a1, const Microsecond & period, int times = INFINITE,
                    const State & state = READY, const Criterion & criterion = NORMAL, unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, SUSPENDED, (criterion != NORMAL) ? criterion : Criterion(period), stack_size),
      _semaphore(0), _handler(&_semaphore, this), _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1, class T2>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2, const Microsecond & period, int times = INFINITE,
                    const State & state = READY, const Criterion & criterion = NORMAL, unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, a2, SUSPENDED, (criterion != NORMAL) ? criterion : Criterion(period), stack_size),
      _semaphore(0), _handler(&_semaphore, this), _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1, class T2, class T3>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3, const Microsecond & period, int times = INFINITE,
                    const State & state = READY, const Criterion & criterion = NORMAL, unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, a2, a3, SUSPENDED, (criterion != NORMAL) ? criterion : Criterion(period), stack_size),
      _semaphore(0), _handler(&_semaphore, this), _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    static volatile bool wait_next() {
        Periodic_Thread * t = reinterpret_cast<Periodic_Thread *>(running());

        if(t->_alarm._times)
            t->_semaphore.p();

        return t->_alarm._times;
    }

protected:
    Semaphore _semaphore;
    Handler _handler;
    Alarm _alarm;
};

class RT_Thread: public Periodic_Thread
{
public:
    enum {
        SAME        = Scheduling_Criteria::RT_Common::SAME,
        NOW         = Scheduling_Criteria::RT_Common::NOW,
        UNKNOWN     = Scheduling_Criteria::RT_Common::UNKNOWN,
        ANY         = Scheduling_Criteria::RT_Common::ANY
    };

public:
    RT_Thread(void (* function)(),
                    const Microsecond & deadline, const Microsecond & period = SAME,
                    const Microsecond & capacity = UNKNOWN, const Microsecond & activation = NOW,
                    int times = INFINITE, int cpu = ANY, unsigned int stack_size = STACK_SIZE)
    : Periodic_Thread(&entry, this, function, activation,
                      activation ? activation : period ? period : deadline,
                      activation ? 1 : times,
                      SUSPENDED,
                      Criterion(deadline, period ? period : deadline, capacity, cpu),
                      stack_size) {
        if(activation && Criterion::dynamic)
            _link.rank(Criterion::PERIODIC); // Will be adjusted to the correct priority by the update() in the operator()()
        resume();
    }

private:
    static int entry(RT_Thread * t, void (*f)(), const Microsecond a) {
        if(a) {
            // Wait for activation time
            t->_semaphore.p();

            // Adjust alarm's period
            t->_alarm.~Alarm();
            new (&t->_alarm) Alarm(t->criterion()._period, &t->_handler, t->_alarm._times);
        }

        // Periodic execution loop
        do {
            Alarm::Tick tick;
            if(Traits<Periodic_Thread>::simulate_capacity && t->criterion()._capacity)
                tick = Alarm::_elapsed + Alarm::ticks(t->criterion()._capacity);

            // Release job
            f();

            if(Traits<Periodic_Thread>::simulate_capacity && t->criterion()._capacity)
                while(Alarm::_elapsed < tick);
        } while (wait_next());

        return 0;
    }
};

__END_SYS

#endif
