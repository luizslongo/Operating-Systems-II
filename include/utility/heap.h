// EPOS Heap Utility Declarations

#ifndef __heap_h
#define __heap_h

#include <utility/debug.h>
#include <utility/list.h>
#include <utility/spin.h>

__BEGIN_SYS

// Heap Common Package (actually the non-atomic heap)
class Heap_Common: private Grouping_List<char>
{
public:
    enum Allocator_Type
    {
//        COLOR_0 = 0,
//        COLOR_1 = 1,
//        COLOR_2 = 2,
//        COLOR_3 = 3,
//        COLOR_4 = 4,
//        COLOR_5 = 5,
//        COLOR_6 = 6,
//        COLOR_7 = 7,
//        COLOR_8 = 8,
//        COLOR_9 = 9,
//        COLOR_10 = 10,
//        COLOR_11 = 11,
//        COLOR_12 = 12,
//        COLOR_13 = 13,
//        COLOR_14 = 14,
//        COLOR_15 = 15,
//        COLOR_16 = 16,
        BASE = 1 << (8 * sizeof(int) - 1),
        HIGH = BASE,
//        LOW,
//        NORMAL,
//        MULTIPLE_WRITERS,
//        SINGLE_WRITER
    };

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

        if(!Traits<CPU>::unaligned_memory_access)
            while((bytes % sizeof(void *)) != 0)
                ++bytes;

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
                      << ") => " << reinterpret_cast<void *>(addr) << "\n";

        addr[0] = bytes;
        return &addr[1];
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
class Heap: public Heap_Wrapper<Traits<Thread>::multicore> {};

__END_SYS

#endif
