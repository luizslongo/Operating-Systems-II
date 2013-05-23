// EPOS Semaphore Abstraction Implementation

#include <semaphore.h>

__BEGIN_SYS

Semaphore::Semaphore(int v): _value(v)
{
    db<Synchronizer>(TRC) << "Semaphore(value=" << _value << ") => "
                          << this << "\n";
}

Semaphore::~Semaphore()
{
    db<Synchronizer>(TRC) << "~Semaphore(this=" << this << ")\n";
}

void Semaphore::p()
{
    db<Synchronizer>(TRC) << "Semaphore::p(this=" << this
                          << ",value=" << _value << ")\n";

    fdec(_value);
    while(_value < 0)
        sleep();
}

void Semaphore::v()
{
    db<Synchronizer>(TRC) << "Semaphore::v(this=" << this
                          << ",value=" << _value << ")\n";

    if(finc(_value) < 1)
        wakeup();
}

__END_SYS
