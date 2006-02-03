// EPOS-- System Initializer

#include <machine.h>
#include <system.h>

__BEGIN_SYS

class Init_System
{

public:
    Init_System() {
	db<Init>(TRC) << "\nInit_System()\n";

	// Initialize the machine
	db<Init>(INF) << "Initializing the machine: \n";
	Machine::init();
	db<Init>(INF) << "done!\n\n";

	// Initialize system abstractions 
	db<Init>(INF) << "Initializing system abstractions: \n";
	System::init();
	db<Init>(INF) << "done!\n\n";

	// Initialization continues in init_application
    }
};

// Global object "init_system"  must be constructed first.
Init_System init_system;

__END_SYS
