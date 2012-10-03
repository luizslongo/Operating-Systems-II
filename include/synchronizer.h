// EPOS Synchronizer Abstractions Common Package

#ifndef __synchronizer_h
#define __synchronizer_h

#include <cpu.h>
#include <thread.h>

__BEGIN_SYS

class Synchronizer_Common
{
private:
    static const bool busy_waiting = Traits<Thread>::busy_waiting;    

protected:
    Synchronizer_Common() {}
    ~Synchronizer_Common() { begin_atomic(); wakeup_all(); }

    // Atomic operations
    bool tsl(volatile bool & lock) { return CPU::tsl(lock); }
    int finc(volatile int & number) { return CPU::finc(number); }
    int fdec(volatile int & number) { return CPU::fdec(number); }

    // Thread operations
    void sleep()
    {
        if(!busy_waiting) // configurable feature
            Thread::yield();
    }

    void wakeup()
    {
        if(!busy_waiting) // configurable feature
            ; // a real wakeup comes here
    }
    
    void wakeup_all()
    {
        if(!busy_waiting) // configurable feature
            ; // a real wakeup_all comes here
    }



    void begin_atomic() { Thread::lock(); }
    void end_atomic() { Thread::unlock(); }

};

__END_SYS

#endif
