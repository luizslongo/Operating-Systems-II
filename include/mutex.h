// EPOS-- Mutex Abstraction Declarations

#ifndef __mutex_h
#define __mutex_h

#include <common/synchronizer.h>

__BEGIN_SYS

class Mutex: public Synchronizer_Common
{
private:
    typedef Traits<Mutex> Traits;
    static const Type_Id TYPE = Type<Mutex>::TYPE;

public:
    Mutex() : _locked(false) { db<Mutex>(TRC) << "Mutex()\n"; }
    ~Mutex() { db<Mutex>(TRC) << "~Mutex()\n"; }

    void lock() { 
	db<Mutex>(TRC) << "Mutex::lock()\n";
	while(tsl(_locked))
	    sleep();
    }
    void unlock() { 
	db<Mutex>(TRC) << "Mutex::unlock()\n";
	_locked = false;
	wakeup(); 
    }

    static int init(System_Info *si);

private:
    volatile bool _locked;
};

__END_SYS

#endif
