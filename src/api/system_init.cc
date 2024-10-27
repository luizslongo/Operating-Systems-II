// EPOS System Initialization

#include <system.h>
#include <time.h>
#include <process.h>

__BEGIN_SYS

void System::init()
{
    
    // BARRIER HERE. WHY? DUNNO.
    // Probably cause of Thread::init, ig.
    CPU::smp_barrier();
    // These abstractions are initialized only once (by the bootstrap CPU)
    if(CPU::id() == CPU::BSP) {
        if(Traits<Alarm>::enabled)
            Alarm::init();
    }

    // These abstractions are initialized by all CPUs
    if(Traits<Thread>::enabled)
        Thread::init();
}

__END_SYS
