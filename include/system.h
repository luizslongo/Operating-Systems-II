// EPOS-- Global System Abstraction Declarations

#ifndef __system_h
#define __system_h

#include <utility/heap.h>
#include <machine.h>

__BEGIN_SYS

class System
{
public:
    static System_Info<Machine> * const info() { return _si; }
    static Heap * const heap() { return &_heap; }

    static void init();

private:
    static System_Info<Machine> * _si;
    static Heap _heap;
};

__END_SYS

#endif
