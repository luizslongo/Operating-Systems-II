// EPOS System-level Dynamic Memory Declarations

#ifndef __kmalloc_h
#define __kmalloc_h

#include <system.h>
#include <system/types.h>
#include <utility/string.h>

__BEGIN_SYS

inline void * kmalloc(unsigned int bytes) { 
    return System::heap()->alloc(bytes);
}

inline void * kcalloc(unsigned int n, unsigned int bytes) {
    void * addr = System::heap()->alloc(n * bytes);
    memset(addr, n * bytes, 0);
    return addr;
}

inline void kfree(void * ptr) {
    System::heap()->free(ptr);
}

__END_SYS

#endif
