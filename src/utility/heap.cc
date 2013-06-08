// EPOS Heap Utility Implementation

#include <utility/heap.h>
#include <machine.h>

__BEGIN_SYS

// Methods
void Simple_Heap::out_of_memory()
{
    db<Heap>(ERR) << "Heap::alloc(this=" << this << "): out of memory!" << endl;

    Machine::panic();
}

__END_SYS
