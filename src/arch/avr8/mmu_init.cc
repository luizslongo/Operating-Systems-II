// EPOS-- AVR8 MMU Mediator Initialization

#include <mmu.h>

__BEGIN_SYS

int AVR8_MMU::init(System_Info * si)
{
    db<AVR8_MMU>(TRC) << "AVR8_MMU::init()\n";

    db<AVR8_MMU>(INF) << "AVR8_MMU:: memory size = " 
		      << si->mem_size << " bytes\n";
    db<AVR8_MMU>(INF) << "AVR8_MMU:: free memory = " 
		      << si->mem_free << " bytes\n";
    db<AVR8_MMU>(INF) << "AVR8_MMU:: application's memory base = " 
		      << (void *) si->pmm.app_lo << "\n";

    _mem_list = new (reinterpret_cast<Mem_List *>(si->pmm.free))
                Mem_List(si->pmm.free + sizeof(Mem_List),
			 si->pmm.free_size - sizeof(Mem_List));

    return 0;
}

__END_SYS

