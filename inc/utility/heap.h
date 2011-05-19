// EPOS Heap Utility Declarations

#ifndef __heap_h
#define __heap_h

#include <utility/debug.h>
#include <utility/string.h>
#include <utility/list.h>
#include <utility/spin.h>

__BEGIN_SYS

// Heap Common Packages (actually the non-atomic heap)
class Heap_Common_No_Profile: private Grouping_List<char>
{
public:
    using Grouping_List<char>::empty;
    using Grouping_List<char>::size;

    Heap_Common_No_Profile() {
	db<Init, Heap>(TRC) << "Heap() => " << this << "\n";
    }

    Heap_Common_No_Profile(void * addr, unsigned int bytes) {
	db<Init, Heap>(TRC) << "Heap(addr=" << addr << ",bytes=" << bytes 
			    << ") => " << this << "\n";  
    }

    void * alloc(unsigned int bytes); 

    void * calloc(unsigned int bytes); 

    void free(void * ptr); 

    void free(void * ptr, unsigned int bytes);

    unsigned int allocated();
    void allocated(unsigned int bytes);

    unsigned int max_depth();
    void max_depth(unsigned int bytes);

private:
    void out_of_memory();
};

//Heap used when priority allocation is enabled
class Heap_Common_Profiled : public Heap_Common_No_Profile {

public:
    Heap_Common_Profiled()
        :Heap_Common_No_Profile(),
         _allocated(0),
         _max_depth(0)
    {}

    Heap_Common_Profiled(void * addr, unsigned int bytes)
        :Heap_Common_No_Profile(addr, bytes),
        _allocated(0),
        _max_depth(0)
    {}

    void * alloc(unsigned int bytes){
        void* aux = Heap_Common_No_Profile::alloc(bytes);
        if(aux){
            _allocated += bytes;
            _max_depth = _allocated > _max_depth ? _allocated : _max_depth;
        }
        return aux;
    }

    void free(void * ptr, unsigned int bytes){
        Heap_Common_No_Profile::free(ptr, bytes);
        _allocated -= bytes;
    }

    void * calloc(unsigned int bytes) {return Heap_Common_No_Profile::calloc(bytes);}

    void free(void * ptr){
        int * addr = reinterpret_cast<int *>(ptr);
        free(&addr[-1], addr[-1]);
    }

    unsigned int allocated() { return _allocated; }
    void allocated(unsigned int bytes) { _allocated = bytes; }

    unsigned int max_depth() { return _max_depth; }
    void max_depth(unsigned int bytes) { _max_depth = bytes; }

private:
    unsigned int _allocated;
    unsigned int _max_depth;
};

//class Heap_Common:
//    public IF<Traits<Heap>::priority_alloc,
//               Heap_Common_Profiled,
//               Heap_Common_No_Profile>::Result
//{};
typedef IF<Traits<Heap>::priority_alloc,
               Heap_Common_Profiled,
               Heap_Common_No_Profile>::Result
        Heap_Common;

// Wrapper for non-atomic heap  
template <bool atomic>
class Heap_Wrapper: public Heap_Common {};


// Wrapper for atomic heap
template<>
class Heap_Wrapper<true>: public Heap_Common
{
public:
    Heap_Wrapper() {}

    Heap_Wrapper(void * addr, unsigned int bytes): Heap_Common(addr, bytes) {
	free(addr, bytes); 
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
