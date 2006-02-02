// EPOS-- Thread Abstraction Initialization

#include <system/kmalloc.h>
#include <system.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

void Thread::init()
{
    int (* entry)() =
	reinterpret_cast<int (*)()>(System::info()->lmm.app_entry);

    db<Init, Thread>(TRC) << "Thread::init(entry="  << entry << ")\n";

    prevent_scheduling();

    if(active_scheduler)
	Alarm::master(QUANTUM, &reschedule);
    
    _running = new(malloc(sizeof(Thread))) Thread(entry, RUNNING);

    _idle = new(kmalloc(sizeof(Thread))) Thread(&idle, READY, IDLE);

    _running->_context->load();

    allow_scheduling();
}

__END_SYS
