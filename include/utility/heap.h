// EPOS-- Heap Utility Declarations

#ifndef __heap_h
#define __heap_h

#include <utility/debug.h>
#include <utility/string.h>
#include <utility/list.h>

__BEGIN_SYS

class Heap: public Grouping_List<char>
{
public:
    Heap() { db<Heap>(TRC) << "Heap::() => " << this << "\n"; }
    Heap(void * addr, unsigned int bytes) {
	db<Heap>(TRC) << "Heap::() => " << this << "\n";  
	free(addr, bytes); 
    }

    void * alloc(unsigned int bytes);
    void * calloc(unsigned int bytes) {
	void * addr = alloc(bytes);
	memset(addr, bytes, 0);
	return addr;	
    }
    void * realloc(void * ptr, unsigned int bytes);

    void free(void * ptr) {
	int * addr = (int *)ptr;
	free(&addr[-1], addr[-1]);
    }
    void free(void * ptr, unsigned int bytes);
};

__END_SYS

#endif
