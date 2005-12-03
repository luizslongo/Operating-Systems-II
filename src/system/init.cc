// EPOS-- Init

// Init is responsible for initializing the operating system. It
// relies on a basic node configuration carried out by SETUP. What Init
// will actually do depends on the OS configuration for the specific
// application.

#include <utility/elf.h>
#include <utility/debug.h>
#include <system/kmalloc.h>
#include <system/init_table.h>
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

class Init
{
private:
    typedef int (Dispatcher) (System_Info * );
    typedef void (Function)(void);

public:
    Init::Init(System_Info * si) {
	db<Init>(TRC) << "Init(si=" << (void *)si << ")\n";

	// Initialize the system's heap
	sys_heap.free(&_sys_heap, Traits<Machine>::SYSTEM_HEAP_SIZE);

	// Double check if we have a heap
	int * tmp = (int *)kmalloc(sizeof(int));
	if(!tmp) {
	    db<Init>(ERR) << "It won't work: we don't have a heap!\n";
	    Machine::panic();
	}
	kfree(tmp);

	// Initialize EPOS abstractions 
	db<Init>(INF) << "Initializing EPOS abstractions: \n";
	for(unsigned int i = 0; init_table[i] != __LAST_INIT; i++)
	    init_table[i](si);
// 	for(unsigned int i = 0; i < LAST_TYPE; i++)
// 	if(Traits< Id<0>::TYPE >::init) 
// 	    Id<0>::TYPE::init(si);
	db<Init>(INF) << "done! \n";

	// Initialize the application's heap
	app_heap.free(
	    MMU::alloc(MMU::pages(Traits<Machine>::APPLICATION_HEAP_SIZE)),
	    Traits<Machine>::APPLICATION_HEAP_SIZE);

	// The system should now be ready to serve applications.
	// The initialization of the Thread abstraction, which will 
	// consequently create the first thread, is carried out by
	// Init_Thread.
    }

private:
    static Dispatcher * init_table[];
};

// Class attributes
Init::Dispatcher * Init::init_table[] = INIT_TABLE;

// Global object init
Init init(si);

__END_SYS
