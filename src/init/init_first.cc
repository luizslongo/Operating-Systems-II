// EPOS-- First Thread Initializer

#include <system.h>
#include <thread.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

class Init_First
{
public:
    Init_First() {
	db<Init>(TRC) << "Init_First()\n";

	System_Info<Machine> * si = System::info();

	// EPOS is a library?
	if(si->bm.system_offset == -1)
	    si->lmm.app_entry =
		reinterpret_cast<unsigned int>(&__epos_library_app_entry);

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
