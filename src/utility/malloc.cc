// EPOS-- Application-level Dynamic Memory Utility Implementation

#include <utility/heap.h>

__BEGIN_SYS
extern Heap app_heap;
__END_SYS

// The preferred implementation of these methods is in <utility/malloc.h>, but 
// GCC implementation of virtual destructors implicitly calls delete, so an 
// extern linkage version is also necessary.

void * operator new(unsigned int bytes) {
    return __SYS(app_heap).alloc(bytes);
}
void * operator new[](unsigned int bytes) { 
    return __SYS(app_heap).alloc(bytes); 
}
void operator delete(void * object) {
    __SYS(app_heap).free(object);
}
