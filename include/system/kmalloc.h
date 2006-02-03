// EPOS-- System-level Dynamic Memory Declarations

#ifndef __kmalloc_h
#define __kmalloc_h

#include <utility/heap.h>
#include <system.h>

__BEGIN_SYS

extern Heap sys_heap;

inline void * kmalloc(unsigned int bytes) { 
    return System::heap()->alloc(bytes);
}
inline void * kcalloc(unsigned int n, unsigned int bytes) {
    return System::heap()->calloc(n * bytes); 
}
inline void kfree(void * ptr) {
    System::heap()->free(ptr); 
}

__END_SYS

#endif
