// EPOS-- AVR8 MMU Mediator Declarations

#ifndef __avr8_mmu_h
#define __avr8_mmu_h

#include <cpu.h>
#include <mmu.h>
#include <utility/string.h>
#include <utility/list.h>
#include __HEADER_MACH(memory_map)

__BEGIN_SYS

class AVR8_MMU: public MMU_Common<0, 0, 0>
{
private:
    typedef Traits<AVR8_MMU> Traits;
    static const Type_Id _TYPE = Type<AVR8_MMU>::TYPE;

    static const unsigned int PHY_MEM = Memory_Map<Machine>::PHY_MEM;
    static const unsigned int SYS_PT  = Memory_Map<Machine>::PHY_MEM;

public:
    // Page Flags
    typedef MMU_Common::Flags AVR8_Flags;

    // Page_Table
    class Page_Table {};

    // Chunk (for Segment)
    class Chunk
    {
    public:
        Chunk() {}
        Chunk(unsigned int bytes, Flags flags)
	    : _phy_addr(alloc(bytes)), _size(bytes), _flags(AVR8_Flags(flags))
	{}
	Chunk(Phy_Addr phy_addr, unsigned int bytes, Flags flags)
	    : _phy_addr(phy_addr), _size(bytes), _flags(AVR8_Flags(flags))
        {}
	~Chunk() {
	    free(_phy_addr, _size);
	}

	unsigned int pts() const { return 0; }
	Flags flags() const { return _flags; }
	Page_Table * pt() const { return 0; }
	unsigned int size() const { return _size; }
	Phy_Addr phy_address() const { return _phy_addr; } // always CT

	int resize(unsigned int amount) { return 0; } // no resize with CT

    private:
        Phy_Addr _phy_addr;
        unsigned int _size;
        AVR8_Flags _flags;
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
	
	Log_Addr attach(const Chunk & chunk) { return chunk.phy_address(); }
	Log_Addr attach(const Chunk & chunk, Log_Addr addr) {
	    return (addr == chunk.phy_address)? addr : false;
	}
 	void detach(const Chunk & chunk) {}
 	void detach(const Chunk & chunk, Log_Addr addr) {}

	Phy_Addr physical(Log_Addr addr) { return addr;	}
    };

public:
    AVR8_MMU() {}

    static void flush_tlb() {}
    static void flush_tlb(Log_Addr addr) {}

    static Phy_Addr alloc(unsigned int bytes = 1);
    static Phy_Addr calloc(unsigned int bytes = 1);
    static void free(Phy_Addr addr, int n = 1);

    static Page_Directory * volatile current() {
	return reinterpret_cast<Page_Directory * volatile>(CPU::pdp());
    }

    static Phy_Addr physical(Log_Addr addr) {
        return addr;
    }

    static int init(System_Info * si);

private:
    static Phy_Addr _base;
    static unsigned int _top;
};

typedef AVR8_MMU MMU;

__END_SYS

#endif
