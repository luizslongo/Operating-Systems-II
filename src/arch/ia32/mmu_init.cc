// EPOS-- IA32 MMU Mediator Initialization

#include <mmu.h>
#include <system.h>

__BEGIN_SYS

void IA32_MMU::init()
{
    System_Info<PC> * si = System::info();

    db<Init, IA32_MMU>(INF) << "IA32_MMU::memory={base=" 
			    << (void *) si->pmm.mem_base << ",size="
			    << (si->bm.mem_top - si->bm.mem_base) / 1024
			    << "KB}\n";
    db<Init, IA32_MMU>(INF) << "IA32_MMU::free1={base=" 
			    << (void *) si->pmm.free1_base << ",size="
			    << (si->pmm.free1_top - si->pmm.free1_base) / 1024
			    << "KB}\n";
    db<Init, IA32_MMU>(INF) << "IA32_MMU::free2={base=" 
			    << (void *) si->pmm.free2_base << ",size="
			    << (si->pmm.free2_top - si->pmm.free2_base) / 1024
			    << "KB}\n";
    
    // BIG WARING HERE: INIT (i.e. this program) will be part of the free
    // storage after the following is executed, but it will remain alive
    // This only works because the _free.insert_merging() only
    // touchs the first page of each chunk and INIT is not there

    // Insert all free memory into the _free list
    List::Element * e, * m1, * m2;
    e = new (phy2log(reinterpret_cast<void *>(si->pmm.free1_base)))
	List::Element(reinterpret_cast<Page *>(si->pmm.free1_base),
		      pages(si->pmm.free1_top - si->pmm.free1_base));
    _free.insert_merging(e, &m1, &m2);

    e = new (phy2log(reinterpret_cast<void *>(si->pmm.free2_base)))
	List::Element(reinterpret_cast<Page *>(si->pmm.free2_base),
		      pages(si->pmm.free2_top - si->pmm.free2_base));
    _free.insert_merging(e, &m1, &m2);

    db<Init, IA32_MMU>(INF) << "IA32_MMU::free pages="
			    << _free.grouped_size() << "\n";

    _master = reinterpret_cast<Page_Directory *>(CPU::pdp());

    db<Init, IA32_MMU>(INF) << "IA32_MMU::master page directory=" 
			    << _master << "\n";

    // Initialize the System's heap
    db<Init, IA32_MMU>(INF) << "IA32_MMU::initializing system's heap="
			    << Traits<Machine>::SYSTEM_HEAP_SIZE 
			    << " bytes.\n";
    System::heap()->free(alloc(pages(Traits<Machine>::SYSTEM_HEAP_SIZE)),
			 Traits<Machine>::SYSTEM_HEAP_SIZE);
}

__END_SYS

