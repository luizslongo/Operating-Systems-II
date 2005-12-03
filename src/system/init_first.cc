// EPOS-- First Thread Initiator

// Init_Thread is responsible for creating the first thread.
// It must be the last global constructor to be executed.

#include <utility/debug.h>
#include <thread.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

extern System_Info * si;

class Init_First
{
public:
    Init_First(System_Info * si) {
	db<Init>(TRC) << "Init_First(si=" << si << ")\n";

	// EPOS is a library?
	if(si->bm.system_off == -1)
	    si->lmm.app_entry =
		reinterpret_cast<unsigned int>(&__epos_library_app_entry);

	// Initialize the Thread abstraction, thus creating the first thread
	db<Init>(INF) << "Starting first process ...\n";
	Thread::init(si);
    }
};

// Global object initializer
Init_First init_first(si);

__END_SYS
