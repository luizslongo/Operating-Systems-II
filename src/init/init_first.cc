// EPOS-- First Thread Initializer

#include <thread.h>



__BEGIN_SYS

class Init_First
{
public:
    Init_First() {
	db<Init>(TRC) << "Init_First()\n";

	// Initialize the Thread abstraction, thus creating the first thread
	db<Init>(INF) << "Starting the first thread ...\n";
	db<Init>(INF) << "Init ends here!\n\n";
	Thread::init();
    }
};

// Global object "init_first" must be constructed last, for it activates the 
// application thread
Init_First init_first;

__END_SYS
