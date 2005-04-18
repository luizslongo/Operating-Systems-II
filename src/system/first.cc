// EPOS-- FIRST

// FIRST is responsible for creating the first process. It is the last global constructor to be executed.

#include <utility/debug.h>
#include <system/first.h>
#include <thread.h>

__BEGIN_SYS

extern System_Info * si;

First::First(System_Info * si)
{
    // Initialize the Thread abstraction, thus creating the first thread
    db<Init>(INF) << "Starting first process ...\n";
    Thread::init(si);

    // This point won't be reached if a member of the Thread family was
    // selected, since the cooresponding initialization will activate the
    // first thread
    reinterpret_cast<Function *>(si->lmm.app_entry)();
}

// Global object initializer
First first(si);

__END_SYS
