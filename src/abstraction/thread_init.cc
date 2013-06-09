// EPOS Thread Abstraction Initialization

#include <system.h>
#include <thread.h>
#include <alarm.h>
#include <task.h>

__BEGIN_SYS

void Thread::init()
{
    int (* entry)() = reinterpret_cast<int (*)()>(System::info()->lmm.app_entry);

    db<Init, Thread>(TRC) << "Thread::init(entry=" << reinterpret_cast<void *>(entry) << ")" << endl;

    // The installation of the scheduler timer handler must precede the
    // creation of threads, since the constructor can induce a reschedule
    // and this in turn can call timer->reset()
    // Letting reschedule() happen during thread creation is harmless, since
    // MAIN is created first and dispatch won't replace it nor by itself
    // neither by IDLE (that has a lower priority)
    if(Criterion::timed && (Machine::cpu_id() == 0))
        _timer = new (SYSTEM) Scheduler_Timer(QUANTUM, time_slicer);

    Machine::smp_barrier();

    Thread * first;

    if(Machine::cpu_id() == 0) {
        if(multitask) {
            // Create the application's main thread
            // This must precede idle, thus avoiding implicit rescheduling
            first = new (SYSTEM) Thread(*Task::_master, entry, RUNNING, MAIN);
            new (SYSTEM) Thread(*Task::_master, &idle, READY, IDLE);
        } else {
            first = new (SYSTEM) Thread(entry, RUNNING, MAIN);
            new (SYSTEM) Thread(&idle, READY, IDLE);
        }
    } else {
        if(multitask)
            first = new (SYSTEM) Thread(*Task::_master, &idle, READY, IDLE);
        else
            first = new (SYSTEM) Thread(&idle, READY, IDLE);
    }
    
    Machine::smp_barrier();

    db<Init, Thread>(INF) << "Dispatching the first thread: " << first << endl;

    This_Thread::not_booting();

    first->_context->load();
}

__END_SYS
