// EPOS Thread Abstraction Initialization

#include <system/kmalloc.h>
#include <system.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

void Thread::init()
{
    int (* entry)() =
	reinterpret_cast<int (*)()>(System::info()->lmm.app_entry);

    db<Init, Thread>(TRC) << "Thread::init(entry=" << (void *)entry << ")\n";

    Machine::smp_barrier();

    if(Machine::cpu_id() == 0) {
        _running = new(kmalloc(sizeof(Thread))) Thread(entry, RUNNING, NORMAL);
    }

    Machine::smp_barrier();

    if(Machine::cpu_id() == 0) {
        if(active_scheduler)
            _timer = new (kmalloc(sizeof(Scheduler_Timer)))
		Scheduler_Timer(QUANTUM, &time_slicer);
    }

    db<Init, Thread>(INF) << "Dispatching the first thread: " << _running << "\n";

    This_Thread::not_booting();

    _running->_context->load();
}

__END_SYS
