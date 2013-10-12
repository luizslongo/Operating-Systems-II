// EPOS Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <utility/string.h>
#include <system.h>
#include <application.h>

extern "C"
{
    // Standard C Library allocators
    inline void * malloc(size_t bytes) {
        if(EPOS::Traits<EPOS::System>::multiheap)
            return EPOS::Application::_heap->alloc(bytes);
        else
            return EPOS::System::_heap->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
        void * ptr = malloc(n * bytes);
        memset(ptr, 0, n * bytes);
        return ptr;
    }

    inline void free(void * ptr) {
        if(EPOS::Traits<EPOS::System>::multiheap)
            EPOS::Heap::typed_free(ptr);
        else
            EPOS::Heap::untyped_free(EPOS::System::_heap, ptr);
    }
}

// C++ dynamic memory allocators and deallocators
inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes);
}

// Delete cannot be declared inline due to virtual destructors
void operator delete(void * ptr);
void operator delete[](void * ptr);

#endif
