// EPOS-- Synchronizer Abstractions Common Package

#ifndef __synchronizer_h
#define __synchronizer_h

#include <cpu.h>
#include <thread.h>

__BEGIN_SYS

class Synchronizer_Common
{
private:
    typedef Ordered_Queue<Thread, Traits<Thread>::smp> Queue;

protected:
    Synchronizer_Common() {}
    ~Synchronizer_Common() { wakeup_all(); }

    bool tsl(volatile bool & lock) { return CPU::tsl(lock); }
    int finc(volatile int & number) { return CPU::finc(number); }
    int fdec(volatile int & number) { return CPU::fdec(number); }

    void sleep() { Thread::sleep(&_queue); }
    void wakeup() { Thread::wakeup(&_queue); }
    void wakeup_all() { Thread::wakeup_all(&_queue); }

private:
    Queue _queue;
};

__END_SYS

#endif
