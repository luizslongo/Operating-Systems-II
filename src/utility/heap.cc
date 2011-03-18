// EPOS Heap Utility Implementation

#include <utility/heap.h>
#include <machine.h>

__BEGIN_SYS

// Methods
void Heap_Common::out_of_memory()
{
    db<Heap>(ERR) << "Heap::alloc(this=" << this
				  << "): out of memory!\n";
    Machine::panic();
}

void * Heap_Common::alloc(unsigned int bytes) {
    if(!bytes)
        return 0;
    
    bytes += sizeof(int);
    if(bytes < sizeof(Element))
        bytes = sizeof(Element);
    
    Element * e = search_decrementing(bytes);
    if(!e) {
        out_of_memory();
        return 0;
    }
    int * addr = reinterpret_cast<int *>(e->object() + e->size());
        
    db<Heap>(TRC) << "Heap::alloc(this=" << this
              << ",bytes=" << bytes 
              << ") => " << (void *)addr << "\n";

    addr[0] = bytes;
    return &addr[1];
}

void * Heap_Common::calloc(unsigned int bytes) {
    void * addr = alloc(bytes);
    memset(addr, bytes, 0);
    return addr;    
}

void Heap_Common::free(void * ptr) {
    int * addr = reinterpret_cast<int *>(ptr);
    free(&addr[-1], addr[-1]);
}

void Heap_Common::free(void * ptr, unsigned int bytes) {
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
