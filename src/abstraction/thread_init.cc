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

    db<Init, Thread>(TRC) << "Thread::init(entry="  << (void *)entry << ")\n";

    prevent_scheduling();

    if(active_scheduler)
	Alarm::master(QUANTUM, &time_reschedule);
    
    // Creates the application's main thread
    // It won't start running because reschedule will find prev == next
    new(kmalloc(sizeof(Thread))) Thread(entry, RUNNING, MAIN);

    // Creates the system's idle thread
    new(kmalloc(sizeof(Thread))) Thread(&idle, READY, IDLE);

    // Loads the first thread's context and go running it
    running()->_context->load();

    allow_scheduling();
}

__END_SYS
