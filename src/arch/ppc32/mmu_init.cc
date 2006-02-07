// EPOS-- PPC32 MMU Mediator Initialization

#include <mmu.h>

__BEGIN_SYS

int PPC32_MMU::init(System_Info * si)
{
    db<PPC32_MMU>(TRC) << "PPC32_MMU::init()\n";

    db<PPC32_MMU>(INF) << "PPC32_MMU:: memory size = " 
                      << si->mem_size << " bytes\n";
    db<PPC32_MMU>(INF) << "PPC32_MMU:: free memory = " 
                      << si->mem_free << " bytes\n";
    db<PPC32_MMU>(INF) << "PPC32_MMU:: application's memory base = " 
                      << (void *) si->pmm.app_lo << "\n";

    PPC32_MMU::free(si->pmm.free, si->pmm.free_size);

    return 0;
}

__END_SYS
