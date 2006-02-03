// EPOS-- Application-level Dynamic Memory Utility Implementation

#include <utility/malloc.h>
#include <mmu.h>

__BEGIN_SYS

Heap app_heap(
    MMU::alloc(MMU::pages(Traits<Machine>::APPLICATION_HEAP_SIZE)),
    Traits<Machine>::APPLICATION_HEAP_SIZE);

__END_SYS
