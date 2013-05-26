// EPOS Global System Abstraction Declarations

#ifndef __system_h
#define __system_h

#include <utility/heap.h>
#include <segment.h>

__BEGIN_SYS

enum System_Allocator
{
    SYSTEM
};

__END_SYS

extern "C"
{
    void * malloc(size_t);
    void free(void *);
}

inline void * operator new(size_t, const EPOS::System_Allocator &);
inline void * operator new[](size_t, const EPOS::System_Allocator &);

__BEGIN_SYS

class System
{
    friend class Init_System;
    friend class Init_Application;
    friend void * ::malloc(size_t);
    friend void ::free(void *);
    friend void * ::operator new(size_t, const EPOS::System_Allocator &);
    friend void * ::operator new[](size_t, const EPOS::System_Allocator &);
    friend void ::operator delete(void *);
    friend void ::operator delete[](void *);

public:
    static System_Info<Machine> * const info();

private:
    static void init();

private:
    static System_Info<Machine> * _si;
    static Segment _heap_segment;
    static Heap _heap;
};

__END_SYS

inline void * operator new(size_t bytes, const EPOS::System_Allocator & allocator) {
    return EPOS::System::_heap.alloc(bytes);
}

inline void * operator new[](size_t bytes, const EPOS::System_Allocator & allocator) {
    return EPOS::System::_heap.alloc(bytes);
}

#endif
