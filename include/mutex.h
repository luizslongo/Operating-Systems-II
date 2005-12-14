// EPOS-- Mutex Abstraction Declarations

#ifndef __mutex_h
#define __mutex_h

#include <common/synchronizer.h>

__BEGIN_SYS

class Mutex: public Synchronizer_Common
{
public:
    Mutex() : _locked(false) {
	db<Synchronizer>(TRC) << "Mutex() => " << this << "\n"; 
    }
    ~Mutex() {
	db<Synchronizer>(TRC) << "~Mutex(this=" << this << ")\n";
    }

    void lock() { 
	db<Synchronizer>(TRC) << "Mutex::lock(this=" << this << ")\n";
	if(tsl(_locked))
	    sleep();
    }
    void unlock() { 
	db<Synchronizer>(TRC) << "Mutex::unlock(this=" << this << ")\n";
	_locked = false;
	wakeup(); 
    }

    static int init(System_Info * si) { return 0; }

private:
    volatile bool _locked;
};

__END_SYS

#endif
