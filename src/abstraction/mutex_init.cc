// EPOS-- Mutex Abstraction Initialization

#include <mutex.h>

__BEGIN_SYS

int Mutex::init(System_Info * si)
{
    db<Mutex>(TRC) << "Mutex::init()\n";

    return 0;
}

__END_SYS
