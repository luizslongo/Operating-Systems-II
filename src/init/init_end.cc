// EPOS Initializer End

#include "system/config.h"
#include <architecture.h>
#include <system.h>
#include <process.h>

__BEGIN_SYS

// This class purpose is simply to define a well-known ending point for the initialization of the system.
// It activates the first application thread (usually main()).
// It must be linked first so init_end becomes the last constructor in the global's constructor list.

class Init_End
{
public:
    Init_End() {
        CPU::smp_barrier();

        if(!Traits<System>::multithread) {
            CPU::int_enable();
            return;
        }

        db<Init>(INF) << "INIT ends here!" << endl;

        Thread * first = Thread::self();
        
        if (!first) {
            OStream os;
            os << '<' << CPU::id() << "> IS NULL!!\n";
        }


        // This barrier is particularly important, since afterwards the temporary stacks
        // and data structures established by SETUP and announced as "free memory" will indeed be
        // available to user threads.
        CPU::smp_barrier();

        // Interrupts have been disabled at Thread::init() and will be reenabled by CPU::Context::load()
        // but we first reset the timer to avoid getting a time interrupt during load()
        if(Traits<Timer>::enabled)
            Timer::reset();

        first->_context->load();
    }
};

Init_End init_end;

__END_SYS
