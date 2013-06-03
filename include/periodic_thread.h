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
private:
    typedef RTC::Microsecond Microsecond;

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
            if(Criterion::dynamic)
                _thread->criterion().update();

            Semaphore_Handler::operator()();
        }

    private:
        Periodic_Thread * _thread;
    };

    typedef IF<Criterion::dynamic, Dynamic_Handler, Static_Handler>::Result Handler;

public:
    Periodic_Thread(int (* entry)(), 
                    const Microsecond & period,
                    int times = Alarm::INFINITE,
                    const State & state = READY,
                    unsigned int stack_size = STACK_SIZE)
    : Thread(entry, BEGINNING, period, stack_size),
      _semaphore(0),
      _handler(&_semaphore, this),
      _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1>
    Periodic_Thread(int (* entry)(T1 a1), T1 a1,
                    const Microsecond & period,
                    int times = Alarm::INFINITE,
                    const State & state = READY,
                    unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, BEGINNING, period, stack_size),
      _semaphore(0),
      _handler(&_semaphore, this),
      _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1, class T2>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2, 
                    const Microsecond & period,
                    int times = Alarm::INFINITE,
                    const State & state = READY,
                    unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, a2, BEGINNING, period, stack_size),
      _semaphore(0),
      _handler(&_semaphore, this),
      _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    template<class T1, class T2, class T3>
    Periodic_Thread(int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3,
                    const Microsecond & period,
                    int times = Alarm::INFINITE,
                    const State & state = READY,
                    unsigned int stack_size = STACK_SIZE)
    : Thread(entry, a1, a2, a3, BEGINNING, period, stack_size),
      _semaphore(0),
      _handler(&_semaphore, this),
      _alarm(period, &_handler, times) {
        if((state == READY) || (state == RUNNING)) {
            _state = SUSPENDED;
            resume();
        } else
            _state = state;
    }

    static void wait_next() {
        reinterpret_cast<Periodic_Thread *>(self())->_semaphore.p();
    }

private:
    Semaphore _semaphore;
    Handler _handler;
    Alarm _alarm;
};

__END_SYS

#endif
