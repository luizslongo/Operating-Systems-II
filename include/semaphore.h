// EPOS Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <synchronizer.h>

__BEGIN_SYS

class Semaphore: protected Synchronizer_Common
{
public:
    Semaphore(int v = 1) : _value(v) {
        db<Synchronizer>(TRC) << "Semaphore(value=" << _value << ") => "
            << this << "\n";
    }

    ~Semaphore() {
        db<Synchronizer>(TRC) << "~Semaphore(this=" << this << ")\n";
    }

    void p()
    { 
        db<Synchronizer>(TRC) << "Semaphore::p(this=" << this 
                              << ",value=" << _value << ")\n";

        fdec(_value);
        while(_value < 0)
            sleep();
    }

    void v()
    {
        db<Synchronizer>(TRC) << "Semaphore::v(this=" << this
                              << ",value=" << _value << ")\n";

        if(finc(_value) < 1)
            wakeup();
    }

private:
    volatile int _value;
};


__END_SYS

#endif
