// EPOS-- AVR8 MMU Mediator Initialization

#include <mmu.h>

__BEGIN_SYS

int AVR8_MMU::init(System_Info * si)
{
    db<Init, AVR8_MMU>(TRC) << "AVR8_MMU::init()\n";

    db<Init, AVR8_MMU>(INF) << "AVR8_MMU:: memory size = " 
			    << si->mem_size << " bytes\n";
    db<Init, AVR8_MMU>(INF) << "AVR8_MMU:: free memory = " 
			    << si->mem_free << " bytes\n";
    db<Init, AVR8_MMU>(INF) << "AVR8_MMU:: application's memory base = " 
			    << (void *) si->pmm.app_lo << "\n";
    
    AVR8_MMU::free(si->pmm.free, si->pmm.free_size);

    return 0;
}

__END_SYS

