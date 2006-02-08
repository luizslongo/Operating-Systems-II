// EPOS-- PPC32 MMU Mediator Initialization

#include <mmu.h>
#include <system.h>

__BEGIN_SYS

void PPC32_MMU::init()
{
    System_Info<Machine> * si = System::info();

    db<Init, PPC32_MMU>(INF) << "PPC32_MMU::memory={base=" 
			     << (void *) si->pmm.mem_base << ",size="
			     << (si->bm.mem_top - si->bm.mem_base) / 1024
			     << "KB}\n";
    db<Init, PPC32_MMU>(INF) << "PPC32_MMU::free1={base=" 
			     << (void *) si->pmm.free1_base << ",size="
			     << (si->pmm.free1_top - si->pmm.free1_base) / 1024
			     << "KB}\n";
    db<Init, PPC32_MMU>(INF) << "PPC32_MMU::free2={base=" 
			     << (void *) si->pmm.free2_base << ",size="
			     << (si->pmm.free2_top - si->pmm.free2_base) / 1024
			     << "KB}\n";

    // Insert all free memory into the free list
    free(si->pmm.free1_base, si->pmm.free1_top -si->pmm.free1_base);
    free(si->pmm.free2_base, si->pmm.free2_top -si->pmm.free2_base);

    // Initialize the System's heap
    db<Init, PPC32_MMU>(INF) << "PPC32_MMU::initializing system's heap="
			     << Traits<Machine>::SYSTEM_HEAP_SIZE 
			     << " bytes.\n";
    System::heap()->free(alloc(pages(Traits<Machine>::SYSTEM_HEAP_SIZE)),
			 Traits<Machine>::SYSTEM_HEAP_SIZE);
}

__END_SYS
