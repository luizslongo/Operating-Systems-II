// EPOS-- First Thread Initiator

// First is responsible for creating the first thread.
// It must be the last global constructor to be executed.

#include <utility/debug.h>
#include <system/first.h>
#include <thread.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

extern System_Info * si;

First::First(System_Info * si)
{
    // EPOS is a library?
    if(si->bm.system_off == -1)
	si->lmm.app_entry = 
	    reinterpret_cast<unsigned int>(__epos_library_app_entry);

    // Initialize the Thread abstraction, thus creating the first thread
    db<Init>(INF) << "Starting first process ...\n";
    Thread::init(si);
}

// Global object initializer
First first(si);

__END_SYS
