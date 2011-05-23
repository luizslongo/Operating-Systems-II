// EPOS Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <application.h>

extern "C"
{
    inline void * malloc(size_t bytes) {
	return __SYS(Application)::heap()->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
	return __SYS(Application)::heap()->calloc(n * bytes);
    }

    inline void free(void * ptr) {
	__SYS(Application)::heap()->free(ptr);
    }
}

inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes); 
}

inline void operator delete(void * object) {
    free(object);
}

inline void operator delete[](void * object) {
    free(object);
}

#endif
