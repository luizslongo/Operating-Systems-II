// EPOS-- Active Object Abstraction Declarations

#ifndef __active_h
#define __active_h

#include <thread.h>

__BEGIN_SYS

class Active: public Thread
{
public:
    Active() : Thread(&entry, this, Thread::SUSPENDED) {}
    virtual ~Active() {}

    virtual int run() = 0;

    void start() { resume(); }

    static int init(System_Info * si) { return 0; }

private:
    static int entry(Active * runnable) { return runnable->run(); }
};

__END_SYS

#endif
