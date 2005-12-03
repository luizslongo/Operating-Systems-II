// EPOS-- IA32 MMU Mediator Initialization

#include <mmu.h>

__BEGIN_SYS

int IA32_MMU::init(System_Info * si)
{
    db<IA32_MMU>(TRC) << "IA32_MMU::init()\n";

    db<IA32_MMU>(INF) << "IA32_MMU:: memory size = " 
		      << si->mem_size << " pages\n";
    db<IA32_MMU>(INF) << "IA32_MMU:: application's memory base = " 
		      << (void *) si->pmm.app_lo << "\n";
    db<IA32_MMU>(INF) << "IA32_MMU:: free chunk = {base=" 
		      << (void *) si->pmm.mach2 << ",size="
		      << (void *) si->pmm.mach3 << "}\n";
    db<IA32_MMU>(INF) << "IA32_MMU:: free chunk = {base=" 
		      << (void *) si->pmm.free << ",size="
		      << (void *) si->pmm.free_size << "}\n";
//     db<IA32_MMU>(INF) << "IA32_MMU:: physical memory logical address = "
// 		      << (void *) si->lmm.phy_mem << "\n";

//     db<IA32_MMU>(INF) << "IA32_MMU::free => {h=" << (void *)_free.head()
// 		      << ",t=" << (void *)_free.head()
// 		      << ",s=" << _free.size() 
// 		      << ",t=" << (void *)&_free 
// 		      << ",s=" << sizeof(_free) << "}\n";
    
    // BIG WARING HERE: INIT (i.e. this program) will be part of the free
    // after the following is executed, but it will remain alive
    // This only works because the _free.insert_merging() only
    // touchs the first page of each chunk and INIT is not there

    // Insert all free memory into the _free list
    List::Element * e, * m1, * m2;
    e = new (phy2log(reinterpret_cast<void *>(si->pmm.mach2)))
	List::Element(reinterpret_cast<Page *>(si->pmm.mach2),
		      si->pmm.mach3);
    _free.insert_merging(e, &m1, &m2);

    e = new (phy2log(reinterpret_cast<void *>(si->pmm.free)))
	List::Element(reinterpret_cast<Page *>(si->pmm.free),
		      si->pmm.free_size);
    _free.insert_merging(e, &m1, &m2);

    db<IA32_MMU>(INF) << "IA32_MMU:: free pages = "
		      << _free.grouped_size() << "\n";

    _master = reinterpret_cast<Page_Directory *>(CPU::pdp());

    db<IA32_MMU>(INF) << "IA32_MMU:: master page directory = " 
		      << _master << "\n";

    return 0;
}

__END_SYS

