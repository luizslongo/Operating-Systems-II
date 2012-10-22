// EPOS Application-level Dynamic Memory Utility Declarations

#ifndef __malloc_h
#define __malloc_h

#include <utility/string.h>
#include <application.h>

extern "C"
{
    // Standard C Library allocators
    inline void * malloc(size_t bytes) {
        return __SYS(Application)::heap()->alloc(bytes);
    }

    inline void * calloc(size_t n, unsigned int bytes) {
        void * addr = malloc(n * bytes);
        memset(addr, n * bytes, 0);
        return addr;
    }

    inline void free(void * ptr) {
        __SYS(Application)::heap()->free(ptr);
    }

    // Typed allocators
    inline void * tmalloc(size_t bytes, __SYS(Heap::Allocator_Type) type) {
        int * tmp;

        switch(type) {
        case __SYS(Heap)::HIGH:
            tmp = reinterpret_cast<int *>(__SYS(Application)::heap()->
                alloc(bytes + sizeof (void *)));
            break;
        }

        tmp[0] = reinterpret_cast<int>(__SYS(Application)::heap());
        return &tmp[1];
    }

    inline void * tcalloc(size_t n, unsigned int bytes, __SYS(Heap::Allocator_Type) type) {
        void * addr = tmalloc(n * bytes, type);
        memset(addr, n * bytes, 0);
        return addr;
    }

    inline void tfree(void * ptr) {
        int * tmp = reinterpret_cast<int *>(ptr);

        reinterpret_cast<__SYS(Heap) *>(tmp[-1])->free(reinterpret_cast<void *>(tmp[-1]));
    }
}


inline void * operator new(size_t bytes, const __SYS(Heap::Allocator_Type) & type) {
    return tmalloc(bytes, type);
}

inline void * operator new[](size_t bytes,  const __SYS(Heap::Allocator_Type) & type) {
    return tmalloc(bytes, type);
}

inline void * operator new(size_t bytes) {
    return malloc(bytes);
}

inline void * operator new[](size_t bytes) {
    return malloc(bytes);
}

inline void operator delete(void * object) {
    if(reinterpret_cast<int>(object) < __SYS(Heap::BASE))
        free(object);
    else
        tfree(object);
}

inline void operator delete[](void * object) {
    if(reinterpret_cast<int>(object) < __SYS(Heap::BASE))
        free(object);
    else
        tfree(object);
}

#endif
