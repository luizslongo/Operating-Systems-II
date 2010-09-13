// EPOS Application-level Dynamic Memory Utility Declarations

#pragma interface

#include <application.h>

#ifndef __malloc_h
#define __malloc_h

inline void * malloc(unsigned int bytes) { 
    return __SYS(Application)::heap()->alloc(bytes);
}
inline void * calloc(unsigned int n, unsigned int bytes) {
    return __SYS(Application)::heap()->calloc(n * bytes); 
}
inline void free(void * ptr) {
    __SYS(Application)::heap()->free(ptr); 
}

inline void * operator new(unsigned int bytes) {
    return malloc(bytes);
}
inline void * operator new[](unsigned int bytes) {
    return malloc(bytes); 
}
inline void operator delete(void * object) {
    free(object);
}
inline void operator delete[](void * object) {
    free(object);
}

#endif
