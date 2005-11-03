// EPOS-- Initializer

// Initializer is responsible for initializing the operating system. It
// relies on a basic node configuration carried out by SETUP. What Initializer
// will actually do depends on the OS configuration for the specific
// application.

#include <utility/elf.h>
#include <utility/debug.h>
#include <system/kmalloc.h>
#include <system/initializer.h>
#include <machine.h>
#include <address_space.h>
#include <segment.h>
#include <mutex.h>
#include <semaphore.h>
#include <condition.h>
#include <alarm.h>
#include <clock.h>
#include <chronometer.h>
#include <network.h>
#include <thread.h>
#include <task.h>

__BEGIN_SYS

extern System_Info * si;
extern char * _sys_heap;

Initializer::Dispatcher * Initializer::init_table[] = INIT_TABLE;

Initializer::Initializer(System_Info * si)
{
    db<Init>(INF)  << "si(" << (void *)si << ")={msize=" << si->mem_size 
		   << ",free=" << si->mem_free << "}\n";

    // Initialize the system's heap
    sys_heap.free(&_sys_heap, Traits<Machine>::SYSTEM_HEAP_SIZE);

    // Double check if we have a heap
    int * tmp = (int *)kmalloc(sizeof(int));
    if(!tmp) {
	db<Init>(ERR) << "It won't work: we don't have a heap!\n";
	CPU::halt();
    } else
	db<Init>(INF) << "heap=" << (void *)tmp << "\n";
    kfree(tmp);

    // Initialize EPOS abstractions 
    db<Init>(INF) << "Initializing EPOS abstractions: \n";
    for(unsigned int i = 0; init_table[i] != __LAST_INIT; i++)
	init_table[i](si);
    db<Init>(INF) << "done! \n";

    // Initialize the application's heap
    app_heap.free(
	MMU::alloc(MMU::pages(Traits<Machine>::APPLICATION_HEAP_SIZE)),
	Traits<Machine>::APPLICATION_HEAP_SIZE);

    // The system should now be ready to serve applications.
    // The initialization of the Thread abstraction, which will consequently 
    // create the first thread, is carried out by First.
}

// Global object initializer
Initializer initializer(si);

__END_SYS
