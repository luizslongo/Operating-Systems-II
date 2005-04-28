// EPOS-- Active Object Abstraction Declarations

#ifndef __active_h
#define __active_h

#include <thread.h>

__BEGIN_SYS

class Active: public Thread
{
private:
    typedef __SYS(Traits)<Active> Traits;
    static const Type_Id TYPE = Type<Active>::TYPE;

public:
    Active() : Thread(&entry, this, Thread::SUSPENDED) {}

    virtual int run() = 0;

    void start() { resume(); }

    static int init(System_Info * si);

private:
    static int entry(Active * runnable) { return runnable->run(); }
};

__END_SYS

#endif
