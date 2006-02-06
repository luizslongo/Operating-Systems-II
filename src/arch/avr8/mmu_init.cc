// EPOS-- AVR8 MMU Mediator Initialization

#include <mmu.h>
#include <system.h>

__BEGIN_SYS

void AVR8_MMU::init()
{
    System_Info<Machine> * si = System::info();

    db<Init, AVR8_MMU>(TRC) << "AVR8_MMU::init()\n";

    db<Init, AVR8_MMU>(INF) << "AVR8_MMU::memory={base=" 
			    << (void *) si->pmm.mem_base << ",size="
			    << (si->bm.mem_top - si->bm.mem_base) / 1024
			    << "KB}\n";
    db<Init, AVR8_MMU>(INF) << "AVR8_MMU::free={base=" 
			    << (void *) si->pmm.free_base << ",size="
			    << (si->pmm.free_top - si->pmm.free_base) / 1024
			    << "KB}\n";
    
    AVR8_MMU::free(si->pmm.free_base, si->pmm.free_top - si->pmm.free_base);

    // Initialize the System's heap
    db<Init, AVR8_MMU>(INF) << "AVR8_MMU::initializing system's heap="
			    << Traits<Machine>::SYSTEM_HEAP_SIZE 
			    << " bytes.\n";
    System::heap()->free(alloc(pages(Traits<Machine>::SYSTEM_HEAP_SIZE)),
			 Traits<Machine>::SYSTEM_HEAP_SIZE);

}

__END_SYS

