// EPOS-- System-level Dynamic Memory Declarations

#ifndef __kmalloc_h
#define __kmalloc_h

#include <utility/heap.h>

__BEGIN_SYS

extern Heap sys_heap;

inline void * kmalloc(unsigned int bytes) { 
    return sys_heap.alloc(bytes);
}
inline void * kcalloc(unsigned int n, unsigned int bytes) {
    return sys_heap.calloc(n * bytes); 
}
inline void * krealloc(void * ptr, unsigned int bytes) {
    return sys_heap.realloc(ptr, bytes);
}
inline void kfree(void * ptr) {
    sys_heap.free(ptr); 
}

// C++ alloc operations have generated too much trouble while being differently
// declared inside and outside the system's namespace and therefore have been
// disabled. In other words, using new and delete for the system heap is no
// longer allowed. Use new (kmalloc(sizeof(T))) T instead.
inline void * operator new(unsigned int bytes);
inline void * operator new[](unsigned int bytes);
inline void operator delete(void * object);

__END_SYS

#endif
