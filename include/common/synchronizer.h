// EPOS-- Synchronizer Abstractions Common Package

#ifndef __synchronizer_h
#define __synchronizer_h

#include <system/config.h>
#include <cpu.h>
#include <thread.h>

__BEGIN_SYS

class Synchronizer_Atomic_Operations
{
public:
    bool tsl(volatile bool & lock) { return CPU::tsl(lock); }
    int finc(volatile int & number) { return CPU::finc(number); }
    int fdec(volatile int & number) { return CPU::fdec(number); }
};

// Busy-waiting
template <bool idle_waiting, bool strict_ordering>
class Synchronizer_Common_Alternatives
    : public Synchronizer_Atomic_Operations
{
protected:
    Synchronizer_Common_Alternatives() {}
    ~Synchronizer_Common_Alternatives() {}

    void sleep() {}
    void wakeup() {}
    void wakeup_all() {}
};

// Idle-waiting, no strict ordering
template <>
class Synchronizer_Common_Alternatives<true, false> 
    : public Synchronizer_Atomic_Operations
{
protected:
    Synchronizer_Common_Alternatives() {}
    ~Synchronizer_Common_Alternatives() {}

    void sleep() { Thread::yield(); }
    void wakeup() {}
    void wakeup_all() {}
};

// Idle-waiting, strict ordering
template <>
class Synchronizer_Common_Alternatives<true, true>
    : public Synchronizer_Atomic_Operations
{
private:
    typedef Queue<Thread> Queue;

protected:
    Synchronizer_Common_Alternatives() : _spin(false) {}
    ~Synchronizer_Common_Alternatives() { wakeup_all(); }

    void sleep() {
	Thread * self = Thread::self();
	Queue::Element e(self);
	while(tsl(_spin));
	_queue.insert(&e);
	_spin = false;
	self->suspend();
    }
    void wakeup() {
	while(tsl(_spin));
	Queue::Element * e = _queue.remove();
	_spin = false;
	if(e)
	    e->object()->resume();
    }
    void wakeup_all() {
	while(tsl(_spin));
	Queue::Element * t = _queue.tail();
	_spin = false;
	
	Queue::Element * e;
	do {
	    while(tsl(_spin));
	    e = _queue.remove();
	    _spin = false;
	    if(e)
		e->object()->resume();
	} while(e != t);
    }

private:
    volatile bool _spin;
    Queue _queue;
};

typedef Synchronizer_Common_Alternatives<Traits<Synchronizer>::idle_waiting, 
					 Traits<Synchronizer>::strict_ordering>
					 Synchronizer_Common;
__END_SYS

#endif
