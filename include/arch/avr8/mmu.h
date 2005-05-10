// EPOS-- AVR8 MMU Mediator Declarations

#ifndef __avr8_mmu_h
#define __avr8_mmu_h

#include <cpu.h>
#include <mmu.h>
#include <utility/string.h>
#include __HEADER_MACH(memory_map)

__BEGIN_SYS

class AVR8_MMU: public MMU_Common<0, 0, 0>
{
private:
    typedef Traits<AVR8_MMU> Traits;
    static const Type_Id _TYPE = Type<AVR8_MMU>::TYPE;

    static const unsigned int PHY_MEM = Memory_Map<Machine>::PHY_MEM;
    static const unsigned int SYS_PT  = Memory_Map<Machine>::PHY_MEM;

    class Mem_List {
    private:
        class Mem_Node {
        public:
	    Mem_Node();
	    Mem_Node(Mem_Node * p, unsigned int s, Mem_Node * n)
	        : _prev(p), _size(s), _next(n) {}
	    void size(unsigned int s) {
	        _size = (_size & 0x8000) | (s & 0x7fff);
	    }
	    unsigned int size() { return (_size & 0x7fff); }
	    void prev(Mem_Node * p) { _prev = p; }
	    Mem_Node * prev() { return _prev; }
	    void next(Mem_Node * n) { _next = n; }
	    Mem_Node * next() { return _next; }
	    bool marked() { return static_cast<bool>(_size & 0x7fff); }
	    void mark() { _size |= 0x8000; }
	    void unmark() { _size &= 0x7fff; }
        private:
	    Mem_Node * _prev;
	    unsigned int _size; //more significative bit is the free flag
	    Mem_Node * _next;
	};

    public:
	Mem_List();

	Mem_List(Phy_Addr addr, unsigned int size)
	    : _head(new (addr)
		    Mem_Node(reinterpret_cast<Mem_Node *>(0),
			     size,
			     reinterpret_cast<Mem_Node *>(0))),
	      _total(size-sizeof(Mem_Node)) {}

	Phy_Addr alloc(unsigned int size) {
	    if(size > _total) return 0;

	    Mem_Node * first = search_unmarked(size,_head);
	    split(first,size);
	    first->mark();
	    _total -= first->size();
	    return Phy_Addr(first + sizeof(Mem_Node));
	}

	void free(Phy_Addr addr) {
	    Mem_Node * n = 
	        reinterpret_cast<Mem_Node *>(
			 static_cast<unsigned int>(addr - sizeof(Mem_Node)));
	    n->unmark();
	    _total += n->size();
	    merge(n);
	}
    private:
	void split(Mem_Node * n, unsigned int size) {
	    if(!(n->size() > (size + sizeof(Mem_Node)))) return;

	    _total -= sizeof(Mem_Node);
	    Mem_Node * nn = reinterpret_cast<Mem_Node *>(n + sizeof(Mem_Node) + n->size());
	    nn->size(n->size() - size - sizeof(Mem_Node));
	    n->size(size);
	    nn->next(n->next());
	    nn->prev(n);
	    n->next(nn);
	}
	void merge(Mem_Node * n) {
	    if(!n->marked()) return;
	    if(!n->next()->marked()) {
	        _total += sizeof(Mem_Node);
	        n->size(n->size() + n->next()->size() + sizeof(Mem_Node));
		n->next(n->next()->next());
		n->next()->next()->prev(n);
	    }
	    if(!n->prev()->marked()) {
	        _total += sizeof(Mem_Node);
	        n = n->prev();
	        n->size(n->size() + n->next()->size() + sizeof(Mem_Node));
		n->next(n->next()->next());
		n->next()->next()->prev(n);
	    }
	}
	Mem_Node * search_unmarked(unsigned int size, Mem_Node * n) {
	    while(1) {
	        if(!n->marked() && n->size() >= size) return n;
		else {
		    n = n->next();
		    if(!n) return n;
		}
	    }
	}
    private:
	Mem_Node * _head;
	unsigned int _total;
    };



public:
    // Page Flags
    class AVR8_Flags {};

    // Page_Table
    class Page_Table {};

    // Chunk (for Segment)
    class Chunk
    {
    public:
        Chunk() {}
        Chunk(unsigned int bytes, Flags flags)
	    : _phy_addr(alloc(bytes)), _size(bytes), _flags(flags)
	{}
	Chunk(Phy_Addr phy_addr, unsigned int bytes, Flags flags)
	    : _phy_addr(phy_addr), _size(bytes), _flags(flags)
        {}
	~Chunk() {
	    free(_phy_addr, _size);
	}

	unsigned int pts() const { return 0; }
	Flags flags() const { return _flags; }
	Page_Table * pt() const { return 0; }
	unsigned int size() const { return _size; }
	Phy_Addr phy_address() const {
	    return _phy_addr;
	}

	int resize(unsigned int amount) {
	    _size += amount;
            return _size;
	}

    private:
        Phy_Addr _phy_addr;
        unsigned int _size;
        Flags _flags;
    };

    // Page Directory
    typedef Page_Table Page_Directory;

    // Directory (for Address_Space)
    class Directory 
    {
    public:
	Directory() {}
	Directory(Page_Directory * pd) {}
	~Directory() {}
	
	Log_Addr attach(const Chunk & chunk) {
	    return chunk.phy_address();
	}
	Log_Addr attach(const Chunk & chunk, Log_Addr addr) {
/*	    if(addr operator==<Log_Addr>(chunk.phy_address)) return addr;
	    return false;*/
	    return addr;
	}
 	void detach(const Chunk & chunk) {}
 	void detach(const Chunk & chunk, Log_Addr addr) {}

	Phy_Addr physical(Log_Addr addr) {
	    return addr;
	}
    };

public:
    AVR8_MMU() {}
    ~AVR8_MMU() {}

    static void flush_tlb() {}
    static void flush_tlb(Log_Addr addr) {}

    static Phy_Addr alloc(unsigned int bytes = 1);
    static Phy_Addr calloc(unsigned int bytes = 1) { return alloc(bytes); }
    static void free(Phy_Addr addr, int n = 1);

    static Page_Directory * volatile current() {
	return 0;
    }

    static Phy_Addr physical(Log_Addr addr) {
        return addr;
    }

    static int init(System_Info * si);

private:
    static Mem_List * _mem_list;
};

typedef AVR8_MMU MMU;

__END_SYS

#endif
