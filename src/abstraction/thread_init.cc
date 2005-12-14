// EPOS-- Thread Abstraction Initialization

#include <system/kmalloc.h>
#include <thread.h>
#include <alarm.h>

__BEGIN_SYS

int Thread::init(System_Info * si)
{
    db<Init>(TRC) << "Thread::init(entry="
		  << (void *)si->lmm.app_entry << ")\n";

    prevent_scheduling();

    if(active_scheduler)
	Alarm::master(QUANTUM, &reschedule);

    _running = 	new(malloc(sizeof(Thread))) 
	Thread(reinterpret_cast<int (*)()>(si->lmm.app_entry), RUNNING);

    _idle = new(kmalloc(sizeof(Thread))) Thread(&idle, READY, IDLE);

    _running->_context->load();

    allow_scheduling();

    return 0;
}

__END_SYS
