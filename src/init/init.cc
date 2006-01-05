// EPOS-- Init

// Init is responsible for initializing the operating system. It
// relies on a basic node configuration carried out by SETUP. What Init
// will actually do depends on the OS configuration for the specific
// application.

#include <utility/elf.h>
#include <utility/debug.h>
#include <system/kmalloc.h>
#include <machine.h>

#include <thread.h>
#include <task.h>
#include <active.h>

#include <address_space.h>
#include <segment.h>

#include <mutex.h>
#include <semaphore.h>
#include <condition.h>

#include <alarm.h>
#include <clock.h>
#include <chronometer.h>

#include <network.h>

__BEGIN_SYS

extern System_Info * si;

template <int id>
inline static void call_init(System_Info * si)
{
    if(Traits<typename Id2Type<id>::TYPE >::initialize)
	Id2Type<id>::TYPE::init(si);
    call_init<id + 1>(si);
};

template <> // Thread is initialized in init_first.cc
inline static void call_init<THREAD_ID>(System_Info * si)
{
    if(THREAD_ID != LAST_TYPE_ID)
	call_init<THREAD_ID + 1>(si);
};

template <>
inline static void call_init<LAST_TYPE_ID>(System_Info * si)
{ 
    if(Traits<Id2Type<LAST_TYPE_ID>::TYPE >::initialize)
	Id2Type<LAST_TYPE_ID>::TYPE::init(si);
};

class Init
{
private:
    typedef int (Dispatcher) (System_Info * );
    typedef void (Function)(void);

public:
    Init::Init(System_Info * si) {
	db<Init>(TRC) << "Init()\n";

	// Double check if we have a heap
	int * tmp = (int *)kmalloc(sizeof(int));
	if(!tmp) {
	    db<Init>(ERR) << "It won't work: we don't have a heap!\n";
	    Machine::panic();
	}
	kfree(tmp);

	// Initialize EPOS abstractions 
	db<Init>(INF) << "Initializing EPOS abstractions: \n";
	call_init<0>(si);
	db<Init>(INF) << "done!\n\n";

	// Initialize the application's heap
	app_heap.free(
	    MMU::alloc(MMU::pages(Traits<Machine>::APPLICATION_HEAP_SIZE)),
	    Traits<Machine>::APPLICATION_HEAP_SIZE);

	// The system should now be ready to serve applications.
	// The initialization of the Thread abstraction, which will 
	// consequently create the first thread, is carried out by
	// Init_Thread.
    }
};

// Global object init
Init init(si);

__END_SYS
