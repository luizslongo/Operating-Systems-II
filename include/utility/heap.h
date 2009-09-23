// EPOS-- Heap Utility Declarations

#ifndef __heap_h
#define __heap_h

#include <utility/debug.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/spin.h>

__BEGIN_SYS

// Heap Common Package (actually the non-atomic heap)
class Heap_Common: private Grouping_List<char>
{
public:
    using Grouping_List<char>::empty;
    using Grouping_List<char>::size;

    Heap_Common() {
	db<Init, Heap>(TRC) << "Heap() => " << this << "\n";
    }

    Heap_Common(void * addr, unsigned int bytes) {
	db<Init, Heap>(TRC) << "Heap(addr=" << addr << ",bytes=" << bytes 
			    << ") => " << this << "\n";  
    }

    void * alloc(unsigned int bytes) {
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

    void * calloc(unsigned int bytes) {
	void * addr = alloc(bytes);
	memset(addr, bytes, 0);
	return addr;	
    }

    void free(void * ptr) {
	int * addr = reinterpret_cast<int *>(ptr);
	free(&addr[-1], addr[-1]);
    }

    void free(void * ptr, unsigned int bytes) {
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

private:
    void out_of_memory();
};


// Wrapper for non-atomic heap  
template <bool atomic>
class Heap_Wrapper: public Heap_Common {
public:
    Heap_Wrapper() {}

    Heap_Wrapper(void * addr, unsigned int bytes): Heap_Common(addr, bytes) {
	free(addr, bytes); 
    }
};


// Wrapper for atomic heap
template<>
class Heap_Wrapper<true>: public Heap_Common
{
public:
    Heap_Wrapper() {}

    Heap_Wrapper(void * addr, unsigned int bytes): Heap_Common(addr, bytes) {
	_lock.acquire();
	free(addr, bytes); 
	_lock.release();
    }

    void * alloc(unsigned int bytes) {
	_lock.acquire();
	void * tmp = Heap_Common::alloc(bytes);
	_lock.release();
	return tmp;
    }

    void * calloc(unsigned int bytes) {
	_lock.acquire();
	void * tmp = Heap_Common::calloc(bytes);
	_lock.release();
	return tmp;
    }

    void free(void * ptr) {
	_lock.acquire();
	Heap_Common::free(ptr);
	_lock.release();
    }

    void free(void * ptr, unsigned int bytes) {
	_lock.acquire();
	Heap_Common::free(ptr, bytes);
	_lock.release();
    }

private:
    Spin _lock;
};


// Heap
class Heap: public Heap_Wrapper<Traits<Thread>::smp> {};

__END_SYS

#endif
