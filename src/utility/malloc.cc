// EPOS Application-level Dynamic Memory Utility Implementation

#define __malloc_h

#include <utility/malloc.h>
#include <application.h>

__USING_SYS

extern "C"
{
    void * malloc(size_t bytes) {
	return Application::heap()->alloc(bytes);
    }

    void * calloc(size_t n, unsigned int bytes) {
	return Application::heap()->calloc(n * bytes);
    }

    void free(void * ptr) {
	Application::heap()->free(ptr);
    }
}

