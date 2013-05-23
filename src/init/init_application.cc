// EPOS Application Initializer

#include <utility/heap.h>
#include <mmu.h>
#include <machine.h>
#include <application.h>

__BEGIN_SYS

class Init_Application
{
public:
    Init_Application() {
	db<Init>(TRC) << "\nInit_Application()\n";

	// Initialize Application's heap
	db<Init>(INF) << "Initializing application's heap \n";
	Application::heap()->
	    free(MMU::alloc(MMU::pages(Traits<Application>::HEAP_SIZE)),
		 Traits<Application>::HEAP_SIZE);

	db<Init>(INF) << "done!\n\n";
    }
};

// Global object "init_application"  must be linked to the application (not 
// to the system) and there constructed at first.
Init_Application init_application;

__END_SYS
