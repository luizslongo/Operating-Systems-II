// EPOS Mutex Abstraction Declarations

#ifndef __mutex_h
#define __mutex_h

#include <synchronizer.h>

__BEGIN_SYS

class Mutex: protected Synchronizer_Common
{
public:
    Mutex(): _locked(false) {
        db<Synchronizer>(TRC) << "Mutex() => " << this << "\n";
    }

    ~Mutex() {
        db<Synchronizer>(TRC) << "~Mutex(this=" << this << ")\n";
    }

    void lock() { 
        db<Synchronizer>(TRC) << "Mutex::lock(this=" << this << ")\n";

        while(tsl(_locked))
            sleep(); // implicit end_atomic()
    }

    void unlock() { 
        db<Synchronizer>(TRC) << "Mutex::unlock(this=" << this << ")\n";

        _locked = false;
        wakeup(); // implicit end_atomic()
    }

private:
    volatile bool _locked;
};

__END_SYS

#endif
