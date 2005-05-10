// EPOS-- AVR8 MMU Mediator Implementation

#include <arch/avr8/mmu.h>

__BEGIN_SYS

// Class attributes
AVR8_MMU::Mem_List * AVR8_MMU::_mem_list;

AVR8_MMU::Phy_Addr AVR8_MMU::alloc(unsigned int bytes) {
    db<AVR8_MMU>(TRC) << "AVR8_MMU::alloc(bytes=" << bytes << ")\n"; 

    return _mem_list->alloc(bytes);
}

void AVR8_MMU::free(Phy_Addr addr, int n) {
    db<AVR8_MMU>(TRC) << "AVR8_MMU::free(addr=" << (void *)addr 
		    << ",n=" << n << ")\n";

    _mem_list->free(addr);
}

__END_SYS
