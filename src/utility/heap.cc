// EPOS-- Heap Utility Implementation

#include <utility/heap.h>
#include <machine.h>

__BEGIN_SYS

// Methods
void * Heap::alloc(unsigned int bytes)
{
    if(!bytes)
	return 0;
	
    bytes += sizeof(int);
    Element * e = search_decrementing(bytes);
    if(!e) {
	db<Heap>(ERR) << "Heap::alloc: out of memory!\n";
	Machine::panic();
	return 0;
    }
    int * addr = reinterpret_cast<int *>(e->object() + e->size());
	    
    db<Heap>(TRC) << "Heap::alloc(this=" << this
		  << ",bytes=" << bytes 
		  << ") => " << (void *)addr << "\n";
    addr[0] = bytes;
    return &addr[1];
}

void Heap::free(void * ptr, unsigned int bytes)
{
    db<Heap>(TRC) << "Heap::free(this=" << this
		  << ",ptr=" << ptr
		  << ",bytes=" << bytes << ")\n";

    if(ptr && (bytes >= sizeof(Element))) {
	Element * e = new (ptr)
	    Element(reinterpret_cast<char *>(ptr), bytes);
	Element * m1, * m2;
	insert_merging(e, &m1, &m2);
    }
}

__END_SYS
