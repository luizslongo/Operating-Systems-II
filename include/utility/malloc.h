// EPOS-- Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <utility/heap.h>

__BEGIN_SYS
extern Heap app_heap;
__END_SYS

inline void * malloc(unsigned int bytes) { 
    return __SYS(app_heap).alloc(bytes);
}
inline void * calloc(unsigned int n, unsigned int bytes) {
    return __SYS(app_heap).calloc(n * bytes); 
}
inline void * realloc(void * ptr, unsigned int bytes) {
    return __SYS(app_heap).realloc(ptr, bytes);
}
inline void free(void * ptr) {
    __SYS(app_heap).free(ptr); 
}

inline void * operator new(unsigned int bytes) {
    return __SYS(app_heap).alloc(bytes);
}
inline void * operator new[](unsigned int bytes) { 
    return __SYS(app_heap).alloc(bytes); 
}
inline void * operator new(unsigned int s, void * a) {
    return a;
}
inline void operator delete(void * object) {
    __SYS(app_heap).free(object);
}

#endif
