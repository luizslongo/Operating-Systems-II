// EPOS-- Semaphore Abstraction Initialization

#include <semaphore.h>

__BEGIN_SYS

int Semaphore::init(System_Info * si)
{
    db<Semaphore>(TRC) << "Semaphore::init()\n";

    return 0;
}

__END_SYS
