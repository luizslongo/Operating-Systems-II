// EPOS Heap Utility Implementation

#include <utility/heap.h>
#include <machine.h>

__BEGIN_SYS

// Methods
void Heap::out_of_memory()
{
    db<Heap>(ERR) << "Heap::alloc(this=" << this
                  << "): out of memory!\n";

    Machine::panic();
}

__END_SYS
