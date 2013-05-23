// EPOS Mutex Abstraction Implementation

#include <mutex.h>

__BEGIN_SYS

Mutex::Mutex(): _locked(false)
{
    db<Synchronizer>(TRC) << "Mutex() => " << this << "\n";
}

Mutex::~Mutex()
{
    db<Synchronizer>(TRC) << "~Mutex(this=" << this << ")\n";
}

void Mutex::lock()
{
    db<Synchronizer>(TRC) << "Mutex::lock(this=" << this << ")\n";

    while(tsl(_locked))
        sleep(); // implicit end_atomic()
}


void Mutex::unlock()
{
    db<Synchronizer>(TRC) << "Mutex::unlock(this=" << this << ")\n";

    _locked = false;
    wakeup(); // implicit end_atomic()
}

__END_SYS
