// EPOS-- Semaphore Abstraction Declarations

#ifndef __semaphore_h
#define __semaphore_h

#include <utility/handler.h>
#include <common/synchronizer.h>

__BEGIN_SYS

class Semaphore: public Synchronizer_Common
{
private:
    typedef Traits<Semaphore> Traits;
    static const Type_Id TYPE = Type<Semaphore>::TYPE;

public:
    Semaphore(int v = 1) : _value(v) {
	db<Semaphore>(TRC) << "Semaphore(value= " << _value << ") => "
			   << this << "\n";
    }
    ~Semaphore() {
	db<Semaphore>(TRC) << "~Semaphore(this=" << this << ")\n";
    }

    void p() { 
	db<Semaphore>(TRC) << "Semaphore::p(this=" << this 
			   << ",value=" << _value << ")\n";
	if(fdec(_value) < 1)
	    while(_value < 0)
		sleep();
    }
    void v() {
	db<Semaphore>(TRC) << "Semaphore::v(this=" << this
			   << ",value=" << _value << ")\n";
	if(finc(_value) < 0)
	    wakeup();
    }

    static int init(System_Info *si);

private:
    volatile int _value;
};

class Handler_Semaphore: public Handler
{
public:
    Handler_Semaphore(Semaphore * h) : _handler(h) {}

    void operator()() { _handler->v(); }
	
private:
    Semaphore * _handler;
};

__END_SYS

#endif
