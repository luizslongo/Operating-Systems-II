// EPOS-- Handler Utility Declarations

#ifndef __handler_h
#define __handler_h

#include <system/config.h>

__BEGIN_SYS

class Handler
{
public:
    // A handler function
    typedef void (Function)();

public:
    Handler() {}
    virtual ~Handler() {}

    virtual void operator()() = 0;

    // Virtual destructors (as genereated by GCC) implicitly call delete, thus
    // causing an undesirable dependency to the libc. Since there is nothing
    // delete here, an empty operator delete was used.
    void operator delete(void *) { }
};

class Handler_Function: public Handler
{
public:
    Handler_Function(Function * h) : _handler(h) {}

    void operator()() { _handler(); }
	
private:
    Function * _handler;
};

__END_SYS

#endif
