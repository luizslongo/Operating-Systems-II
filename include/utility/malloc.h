// EPOS Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <utility/string.h>
#include <application.h>

extern "C"
{
    // Standard C Library allocators
    inline void * malloc(size_t bytes) {
        return EPOS::Application::heap()->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
        void * ptr = malloc(n * bytes);
        memset(ptr, 0, n * bytes);
        return ptr;
    }

    inline void free(void * ptr) {
        return EPOS::Application::heap()->free(ptr);
    }
}

// C++ dynamic memory allocators and deallocators
inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes);
}

inline void operator delete(void * object) {
    return free(object);
}

inline void operator delete[](void * object) {
    return free(object);
}

#endif
