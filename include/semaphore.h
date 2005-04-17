// EPOS-- Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <common/synchronizer.h>

__BEGIN_SYS

class Semaphore: public Synchronizer_Common
{
private:
    typedef Traits<Semaphore> Traits;
    static const Type_Id TYPE = Type<Semaphore>::TYPE;

public:
    Semaphore(int v = 1) : _value(v) {
	db<Semaphore>(TRC) << "Semaphore(value= " << _value << ")\n";
    }
    ~Semaphore() {
	db<Semaphore>(TRC) << "~Semaphore()\n";
    }

    void p() { 
	db<Semaphore>(TRC) << "Semaphore::p(value=" << _value << ")\n";
	while(fdec(_value) < 0)
	    sleep();
    }
    void v() {
	db<Semaphore>(TRC) << "Semaphore::v(value=" << _value << ")\n";
	if(finc(_value) < 1)
	    wakeup();
    }

    static int init(System_Info *si);

private:
    volatile int _value;
};

__END_SYS

#endif
