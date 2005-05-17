// EPOS-- Mutex Abstraction Declarations

#ifndef __mutex_h
#define __mutex_h

#include <common/synchronizer.h>

__BEGIN_SYS

class Mutex: public Synchronizer_Common
{
private:
    typedef Traits<Synchronizer> Traits;
    static const Type_Id TYPE = Type<Mutex>::TYPE;

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

    static int init(System_Info * si);

private:
    volatile bool _locked;
};

__END_SYS

#endif
