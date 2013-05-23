// EPOS Global Application Abstraction Declarations

#ifndef __application_h
#define __application_h

#include <utility/heap.h>

__BEGIN_SYS

class Application
{
public:
    static Heap * const heap() { return &_heap; }

    static void init();

private:
    static Heap _heap;
};

__END_SYS

#include <utility/malloc.h>

#endif
