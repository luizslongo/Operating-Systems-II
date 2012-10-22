// EPOS Global Application Abstraction Declarations

#ifndef __application_h
#define __application_h

#include <utility/heap.h>

__BEGIN_SYS

class Application
{
public:
    static Heap * const heap(const Heap::Allocator_Type & t = Heap::BASE) {
        return &_heap[t - Heap::BASE];
    }

    static void init();

private:
    static Heap _heap[Traits<Machine>::N_HEAPS];
};

__END_SYS

#include <utility/malloc.h>

#endif
