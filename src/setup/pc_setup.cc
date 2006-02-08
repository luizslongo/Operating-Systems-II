// EPOS-- PC SETUP

#include <utility/elf.h>
#include <utility/string.h>
#include <utility/ostream.h>
#include <utility/debug.h>
#include <machine.h>

ASMV("jmp _start");

__BEGIN_SYS

OStream kout, kerr;

//========================================================================
// PC_Setup
//
// PC_Setup is responsible for bringing the machine into a usable state. It
// sets up several IA32 data structures (IDT, GDT, etc), builds a basic
// memory model (flat) and a basic thread model (exclusive task/exclusive
// thread). 
//------------------------------------------------------------------------
class PC_Setup
{
private:
    static const unsigned int HARD_INT = Traits<PC>:: HARDWARE_INT_OFFSET;
    static const unsigned int BOOT_IMAGE_ADDR = Traits<PC>::BOOT_IMAGE_ADDR;
    static const unsigned int SYS_STACK_SIZE = Traits<PC>::SYSTEM_STACK_SIZE;

    static const unsigned int SYS_INFO = Memory_Map<PC>::SYS_INFO;
    static const unsigned int IDT = Memory_Map<PC>::IDT;
    static const unsigned int GDT = Memory_Map<PC>::GDT;
    static const unsigned int PHY_MEM = Memory_Map<PC>::PHY_MEM;
    static const unsigned int IO_MEM = Memory_Map<PC>::IO_MEM;
    static const unsigned int SYS_PT = Memory_Map<PC>::SYS_PT;
    static const unsigned int SYS_PD = Memory_Map<PC>::SYS_PD;
    static const unsigned int SYS = Memory_Map<PC>::SYS;
    static const unsigned int SYS_DATA = Memory_Map<PC>::SYS_DATA;
    static const unsigned int SYS_CODE = Memory_Map<PC>::SYS_CODE;
    static const unsigned int SYS_STACK = Memory_Map<PC>::SYS_STACK;
    
    // IA32 Imports
    typedef CPU::Reg32 Reg32;
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::Log_Addr Log_Addr;
    typedef CPU::GDT_Entry GDT_Entry;
    typedef CPU::IDT_Entry IDT_Entry;
    typedef MMU::Page Page;
    typedef MMU::Page_Table Page_Table;
    typedef MMU::Page_Directory Page_Directory;
    typedef MMU::PT_Entry PT_Entry;
    typedef MMU::IA32_Flags Flags;

    // System_Info Imports
    typedef System_Info<PC>::Boot_Map BM;
    typedef System_Info<PC>::Physical_Memory_Map PMM;
    typedef System_Info<PC>::Logical_Memory_Map LMM;
    typedef System_Info<PC>::Load_Map LM;

public:
    PC_Setup(char * boot_image) {

	// Get boot image loaded by the bootstrap
 	bi = reinterpret_cast<char *>(boot_image);
	si = reinterpret_cast<System_Info<PC> *>(bi);

 	db<Setup>(TRC) << "PC_Setup(bi=" << (void *)bi
 		       << ",sp=" << (void *)CPU::sp() << ")\n";

	// Disable hardware interrupts
	IC::init();
  	CPU::int_disable();

	build_lm();
	build_pmm();
	build_lmm();
	get_node_id();

	say_hi();

	activate_paging();

	// Adjust pointers that will still be used to their logical addresses
	bi = reinterpret_cast<char *>(unsigned(bi) | PHY_MEM);
	si = reinterpret_cast<System_Info<PC> *>(SYS_INFO);

	load_parts();

	// SETUP ends here, transfer control to next stage (INIT or APP)
	call_next();

	// SETUP is now part of the free memory and this point should never be
	// reached, but, just in case ... :-)
	panic();
    }

private:
    void build_lm();
    void build_pmm();
    void build_lmm();
    void get_node_id();

    void say_hi();

    void activate_paging();
    void setup_idt();
    void setup_gdt();
    void setup_sys_pt();
    void setup_sys_pd();

    void load_parts();
    void call_next();

    void pci_aperture(unsigned int * base, unsigned int * top);

    static void panic() { Machine::panic(); }

private:
    char * bi;
    System_Info<PC> * si;
};

//========================================================================
void PC_Setup::build_lm()
{
    // Get boot image structure
    si->lm.has_stp = (si->bm.setup_offset != -1);
    si->lm.has_ini = (si->bm.init_offset != -1);
    si->lm.has_sys = (si->bm.system_offset != -1);
    si->lm.has_app = (si->bm.application_offset != -1);
    si->lm.has_ext = (si->bm.extras_offset != -1);

    // Check SETUP integrity and get the size of its segments
    si->lm.stp_entry = 0;
    si->lm.stp_code = ~0U;
    si->lm.stp_code_size = 0;
    si->lm.stp_data = ~0U;
    si->lm.stp_data_size = 0;
    if(si->lm.has_stp) {
	ELF * stp_elf = reinterpret_cast<ELF *>(&bi[si->bm.setup_offset]);
	if(!stp_elf->valid()) {
	    db<Setup>(ERR) << "SETUP ELF image is corrupted!\n";
	    panic();
	}

	si->lm.stp_entry = stp_elf->entry();
	si->lm.stp_code = stp_elf->segment_address(0);
	si->lm.stp_code_size = stp_elf->segment_size(0);
	if(stp_elf->segments() > 1) {
	    for(int i = 1; i < stp_elf->segments(); i++) {
		if(stp_elf->segment_type(i) != PT_LOAD)
		    continue;
		if(stp_elf->segment_address(i) < si->lm.stp_data)
		    si->lm.stp_data = stp_elf->segment_address(i);
		si->lm.stp_data_size += stp_elf->segment_size(i);
	    }
	}
    }

    // Check INIT integrity and get the size of its segments
    si->lm.ini_entry = 0;
    si->lm.ini_segments = 0;
    si->lm.ini_code = ~0U;
    si->lm.ini_code_size = 0;
    si->lm.ini_data = ~0U;
    si->lm.ini_data_size = 0;
    if(si->lm.has_ini) {
	ELF * ini_elf = reinterpret_cast<ELF *>(&bi[si->bm.init_offset]);
	if(!ini_elf->valid()) {
	    db<Setup>(ERR) << "INIT ELF image is corrupted!\n";
	    panic();
	}

	si->lm.ini_entry = ini_elf->entry();
	si->lm.ini_code = ini_elf->segment_address(0);
	si->lm.ini_code_size = ini_elf->segment_size(0);
	if(ini_elf->segments() > 1) {
	    for(int i = 1; i < ini_elf->segments(); i++) {
		if(ini_elf->segment_type(i) != PT_LOAD)
		    continue;
		if(ini_elf->segment_address(i) < si->lm.ini_data)
		    si->lm.ini_data = ini_elf->segment_address(i);
		si->lm.ini_data_size += ini_elf->segment_size(i);
	    }
	}
    }

    // Check SYSTEM integrity and get the size of its segments
    si->lm.sys_entry = 0;
    si->lm.sys_segments = 0;
    si->lm.sys_code = ~0U;
    si->lm.sys_code_size = 0;
    si->lm.sys_data = ~0U;
    si->lm.sys_data_size = 0;
    si->lm.sys_stack = SYS_STACK;
    si->lm.sys_stack_size = SYS_STACK_SIZE;
    if(si->lm.has_sys) {
	ELF * sys_elf = reinterpret_cast<ELF *>(&bi[si->bm.system_offset]);
	if(!sys_elf->valid()) {
	    db<Setup>(ERR) << "OS ELF image is corrupted!\n";
	    panic();
	}

	si->lm.sys_entry = sys_elf->entry();
	si->lm.sys_code = sys_elf->segment_address(0);
	si->lm.sys_code_size = sys_elf->segment_size(0);
	if(sys_elf->segments() > 1) {
	    for(int i = 1; i < sys_elf->segments(); i++) {
		if(sys_elf->segment_type(i) != PT_LOAD)
		    continue;
		if(sys_elf->segment_address(i) < si->lm.sys_data)
		    si->lm.sys_data = sys_elf->segment_address(i);
		si->lm.sys_data_size += sys_elf->segment_size(i);
	    }
	}

	if(si->lm.sys_code != SYS_CODE) {
	    db<Setup>(ERR) << "OS code segment address do not match "
			   << "the machine's memory map!\n";
	    panic();
	}
	if(si->lm.sys_code + si->lm.sys_code_size > si->lm.sys_data) {
	    db<Setup>(ERR) << "OS code segment is too large!\n";
	    panic();
	}
	if(si->lm.sys_data != SYS_DATA) {
	    db<Setup>(ERR) << "OS code segment address do not match "
			   << "the machine's memory map!\n";
	    panic();
	}
	if(si->lm.sys_data + si->lm.sys_data_size > si->lm.sys_stack) {
	    db<Setup>(ERR) << "OS data segment is too large!\n";
	    panic();
	}
	if(si->lm.sys_data + si->lm.sys_data_size > si->lm.sys_stack) {
	    db<Setup>(ERR) << "OS data segment is too large!\n";
	    panic();
	}
	if(MMU::page_tables(MMU::pages(si->lm.sys_stack
				       - SYS + si->lm.sys_stack_size)) > 1) {
	    db<Setup>(ERR) << "OS stack segment is too large!\n";
	    panic();
	}
    }

    // Check APPLICATION integrity and get the size of its segments
    si->lm.app_entry = 0;
    si->lm.app_segments = 0;
    si->lm.app_code = ~0U;
    si->lm.app_code_size = 0;
    si->lm.app_data = ~0U;
    si->lm.app_data_size = 0;
    if(si->lm.has_app) {
	ELF * app_elf =
	    reinterpret_cast<ELF *>(&bi[si->bm.application_offset]);
	if(!app_elf->valid()) {
	    db<Setup>(ERR) << "Application ELF image is corrupted!\n";
	    panic();
	}
	si->lm.app_entry = app_elf->entry();
	si->lm.app_code = app_elf->segment_address(0);
	si->lm.app_code_size = app_elf->segment_size(0);
	if(app_elf->segments() > 1) {
	    for(int i = 1; i < app_elf->segments(); i++) {
		if(app_elf->segment_type(i) != PT_LOAD)
		    continue;
		if(app_elf->segment_address(i) < si->lm.app_data)
		    si->lm.app_data = app_elf->segment_address(i);
		si->lm.app_data_size += app_elf->segment_size(i);
	    }
	}
    }

    // Check for EXTRA data in the boot image		
    if(si->lm.has_ext) {
	si->lm.ext = Phy_Addr(&bi[si->bm.extras_offset]);
	si->lm.ext_size = si->bm.img_size - si->bm.extras_offset;
    }
}

//========================================================================
void PC_Setup::build_pmm()
{
    // Allocate (reserve) memory for all entities we have to setup.
    // We'll start at the highest address to make possible a memory model
    // on which the application's logical and physical address spaces match.

    Phy_Addr top_page = MMU::pages(si->bm.mem_top);

    // IDT (1 x sizeof(Page))
    top_page -= 1;
    si->pmm.idt = top_page * sizeof(Page);

    // GDT (1 x sizeof(Page))
    top_page -= 1;
    si->pmm.gdt = top_page * sizeof(Page);

    // System Page Table (1 x sizeof(Page))
    top_page -= 1;
    si->pmm.sys_pt = top_page * sizeof(Page);

    // System Page Directory (1 x sizeof(Page))
    top_page -= 1;
    si->pmm.sys_pd = top_page * sizeof(Page);

    // System Info (1 x sizeof(Page))
    top_page -= 1;
    si->pmm.sys_info = top_page * sizeof(Page);

    // Page tables to map the whole physical memory
    // = NP/NPTE_PT * sizeof(Page)
    //   NP = size of physical memory in pages
    //   NPTE_PT = number of page table entries per page table
    unsigned int mem_size = MMU::pages(si->bm.mem_top - si->bm.mem_base);
    top_page -= (mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;
    si->pmm.phy_mem_pts = top_page * sizeof(Page);

    // Page tables to map the IO address space
    // = NP/NPTE_PT * sizeof(Page)
    // NP = size of PCI address space in pages
    // NPTE_PT = number of page table entries per page table
    pci_aperture(&si->pmm.io_mem_base, &si->pmm.io_mem_top);
    unsigned int io_mem_size =
	MMU::pages(si->pmm.io_mem_top - si->pmm.io_mem_base);
    top_page -= (io_mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;
    si->pmm.io_mem_pts = top_page * sizeof(Page);

    // SYSTEM code segment
    top_page -= MMU::pages(si->lm.sys_code_size);
    si->pmm.sys_code = top_page * sizeof(Page);

    // SYSTEM data segment
    top_page -= MMU::pages(si->lm.sys_data_size);
    si->pmm.sys_data = top_page * sizeof(Page);

    // SYSTEM stack segment
    top_page -= MMU::pages(si->lm.sys_stack_size);
    si->pmm.sys_stack = top_page * sizeof(Page);

    // The memory allocated so far will "disapear" from the system as we
    // set mem_top as follows:
    si->pmm.mem_base = si->bm.mem_base;
    si->pmm.mem_top = top_page * sizeof(Page);

    // Free chuncks (passed to MMU::init)
    si->pmm.free1_base =
	MMU::align_page(si->lm.app_code + si->lm.app_code_size);
    si->pmm.free1_top = MMU::align_page(si->lm.app_data);
    si->pmm.free2_base =
	MMU::align_page(si->lm.app_data + si->lm.app_data_size);
    si->pmm.free2_top = MMU::align_page(si->pmm.mem_top);

    if(si->lm.has_ext) {
	si->pmm.ext_base = si->lm.ext;
	si->pmm.ext_top = si->lm.ext + si->lm.ext_size;
    } else {
	si->pmm.ext_base = 0;
	si->pmm.ext_top = 0;
    }	
}

//========================================================================
void PC_Setup::build_lmm()
{
    si->lmm.app_entry = si->lm.app_entry;
}

//========================================================================
void PC_Setup::get_node_id()
{
    // If we didn't get our node's id in the boot image, we'll to try to
    // get if from an eventual BOOPT reply used to boot up the system before
    // we allocate more memory
    // if(si->bm.host_id == (unsigned short) -1)
    // get_bootp_info(&si->bm.host_id);
}

//========================================================================
void PC_Setup::say_hi()
{
    if(!si->lm.has_app) {
	db<Setup>(ERR)
	    << "No APPLICATION in boot image, you don't need EPOS!\n";
	panic();
    }
    if(!si->lm.has_sys)
	db<Setup>(WRN) 
	    << "No SYSTEM in boot image, assuming EPOS is a library!\n";

    kout << "Setting up this machine as follows: \n";
    kout << "  Processor:    IA32\n";
    kout << "  Memory:       " << (si->bm.mem_top - si->bm.mem_base) / 1024
	 << " Kbytes [" << (void *)si->bm.mem_base
	 << ":" << (void *)si->bm.mem_top << "]\n";
    kout << "  User memory:  "
	 << (si->pmm.mem_top - si->pmm.mem_base) / 1024
	 << " Kbytes [" << (void *)si->pmm.mem_base
	 << ":" << (void *)si->pmm.mem_top << "]\n";
    kout << "  PCI aperture: " 
	 << (si->pmm.io_mem_top - si->pmm.io_mem_base) / 1024
	 << " Kbytes [" << (void *)si->pmm.io_mem_base 
	 << ":" << (void *)si->pmm.io_mem_top << "]\n";
    kout << "  Node Id:      ";
    if(si->bm.node_id != -1)
	kout << si->bm.node_id << " (" << si->bm.n_nodes << ")\n";
    else
	kout << "will get from the network!\n";
    if(si->lm.has_stp)
	kout << "  Setup:        "
	     << si->lm.stp_code_size + si->lm.stp_data_size << " bytes\n";
    if(si->lm.has_ini)
	kout << "  Init:         " 
	     << si->lm.ini_code_size + si->lm.ini_data_size << " bytes\n";
    if(si->lm.has_sys) {
	kout << "  OS code:      " << si->lm.sys_code_size << " bytes";
	kout << "\tdata: " << si->lm.sys_data_size << " bytes";
	kout << "\tstack: " << si->lm.sys_stack_size << " bytes\n";
    }
    if(si->lm.has_app) {
	kout << "  APP code:     " << si->lm.app_code_size << " bytes";
	kout << "\tdata: " << si->lm.app_data_size << " bytes\n";
    }
    if(si->lm.has_ext) {
	kout << "  Extras:       " << si->lm.ext_size << " bytes\n";
    }

    // Test if we didn't overlap SETUP and the boot image
    if(si->pmm.mem_top
       <= si->lm.stp_code + si->lm.stp_code_size + si->lm.stp_data_size) {
  	db<Setup>(ERR) << "SETUP would have been overwritten!\n";
  	panic();
    }
}

//========================================================================
void PC_Setup::activate_paging() 
{
    // Setup the IDT
    setup_idt();

    // Setup the GDT
    setup_gdt();

    // Setup the System Page Table
    setup_sys_pt();

    // Setup the System Page Directory and map physical memory
    setup_sys_pd();
   
    // Set IDTR (limit = 1 x sizeof(Page))
    CPU::idtr(sizeof(Page) - 1, IDT);

    // Reload GDTR with its linear address (one more absurd from Intel!)
    CPU::gdtr(sizeof(Page) - 1, GDT);

    // Set CR3 (PDBR) register
    CPU::cr3(si->pmm.sys_pd);

    // Enable paging
    Reg32 aux = CPU::cr0();
    aux &= CPU::CR0_CLEAR;
    aux |= CPU::CR0_SET;
    CPU::cr0(aux);

    db<Setup>(INF) << "CR0=" << (void *)CPU::cr0() << "\n";
    db<Setup>(INF) << "CR3=" << (void *)CPU::cr3() << "\n";

    // The following relative jump is to break the IA32 pre-fetch queue
    // (in case cr0() was a macro and didn't do it when returning)
    // and also to start using logical addresses
    ASM("ljmp %0, %1 + 1f" : : "i"(CPU::SEL_FLT_CODE), "i"(PHY_MEM));
    ASM("1:");

    // Reload segment registers with GDT_FLT_DATA
    ASM("" : : "a" (CPU::SEL_FLT_DATA));
    ASM("movw %ax, %ds");
    ASM("movw %ax, %es");
    ASM("movw %ax, %fs");
    ASM("movw %ax, %gs");
    ASM("movw %ax, %ss");

    // Set stack pointer to its logical address
    ASM("orl %0, %%esp" : : "i" (PHY_MEM));

    // Flush TLB to ensure we've got the right memory organization
    MMU::flush_tlb();
}

//========================================================================
void PC_Setup::setup_idt()
{
    db<Setup>(TRC) << "setup_idt(idt=" << (void *)si->pmm.idt << ")\n";

    // Get the physical address for the IDT
    IDT_Entry * idt = reinterpret_cast<IDT_Entry *>((void *)si->pmm.idt);

    // Clear IDT
//    memset(idt, 0, sizeof(Page));

    // Adjust handler addresses to logical addresses
    Log_Addr panic_h = Log_Addr(&panic) | PHY_MEM;

    // Map all handlers to panic()
    for(unsigned int i = 0; i < CPU::IDT_ENTRIES; i++)
	idt[i] = IDT_Entry(CPU::GDT_SYS_CODE, panic_h, CPU::SEG_IDT_ENTRY);

    db<Setup>(INF) << "IDT[0  ]=" << idt[0]
		   << " (" << panic_h << ")\n";

}

//========================================================================
void PC_Setup::setup_gdt()
{
    db<Setup>(TRC) << "setup_gdt(gdt=" << (void *)si->pmm.gdt << ")\n";

    // Get the physical address for the GDT
    GDT_Entry * gdt = reinterpret_cast<GDT_Entry *>((void *)si->pmm.gdt);

    // Clear GDT
//    memset(gdt, 0, sizeof(Page));

    // GDT_Entry(base, limit, {P,DPL,S,TYPE})
    gdt[CPU::GDT_NULL]      = GDT_Entry(0,       0, 0);
    gdt[CPU::GDT_FLT_CODE]  = GDT_Entry(0, 0xfffff, CPU::SEG_FLT_CODE);
    gdt[CPU::GDT_FLT_DATA]  = GDT_Entry(0, 0xfffff, CPU::SEG_FLT_DATA);
    gdt[CPU::GDT_APP_CODE]  = GDT_Entry(0, 0xfffff, CPU::SEG_APP_CODE);
    gdt[CPU::GDT_APP_DATA]  = GDT_Entry(0, 0xfffff, CPU::SEG_APP_DATA);
    gdt[CPU::GDT_APP_STACK] = GDT_Entry(0, 0xfffff, CPU::SEG_APP_DATA);
    gdt[CPU::GDT_SYS_CODE]  = GDT_Entry(0, 0xfffff, CPU::SEG_SYS_CODE);
    gdt[CPU::GDT_SYS_DATA]  = GDT_Entry(0, 0xfffff, CPU::SEG_SYS_DATA);
    gdt[CPU::GDT_SYS_STACK] = GDT_Entry(0, 0xfffff, CPU::SEG_SYS_DATA);

    db<Setup>(INF) << "GDT[NULL=" << CPU::GDT_NULL << "]="
		   << gdt[CPU::GDT_NULL] << "\n";
    db<Setup>(INF) << "GDT[FLCD=" << CPU::GDT_FLT_CODE << "]="
		   << gdt[CPU::GDT_FLT_CODE] << "\n";
    db<Setup>(INF) << "GDT[FLDT=" << CPU::GDT_FLT_DATA << "]="
		   << gdt[CPU::GDT_FLT_DATA] << "\n";
    db<Setup>(INF) << "GDT[APCD=" << CPU::GDT_APP_CODE << "]="
		   << gdt[CPU::GDT_APP_CODE] << "\n";
    db<Setup>(INF) << "GDT[APDT=" << CPU::GDT_APP_DATA  << "]="
		   << gdt[CPU::GDT_APP_DATA] << "\n";
    db<Setup>(INF) << "GDT[APST=" << CPU::GDT_APP_STACK << "]="
		   << gdt[CPU::GDT_APP_STACK] << "\n";
    db<Setup>(INF) << "GDT[SYCD=" << CPU::GDT_SYS_CODE << "]="
		   << gdt[CPU::GDT_SYS_CODE] << "\n";
    db<Setup>(INF) << "GDT[SYDT=" << CPU::GDT_SYS_DATA << "]="
		   << gdt[CPU::GDT_SYS_DATA] << "\n";
    db<Setup>(INF) << "GDT[SYST=" << CPU::GDT_SYS_STACK << "]="
		   << gdt[CPU::GDT_SYS_STACK] << "\n";
}

//========================================================================
void PC_Setup::setup_sys_pt()
{
    db<Setup>(TRC) << "setup_sys_pt(pmm={idt=" << (void *)si->pmm.idt
		   << ",gdt="  << (void *)si->pmm.gdt
		   << ",pt="   << (void *)si->pmm.sys_pt
		   << ",pd="   << (void *)si->pmm.sys_pd
		   << ",info=" << (void *)si->pmm.sys_info
		   << ",mem="  << (void *)si->pmm.phy_mem_pts
		   << ",io="   << (void *)si->pmm.io_mem_pts
		   << ",sysc=" << (void *)si->pmm.sys_code
		   << ",sysd=" << (void *)si->pmm.sys_data
		   << ",syss=" << (void *)si->pmm.sys_stack
		   << ",memb=" << (void *)si->pmm.mem_base
		   << ",memt=" << (void *)si->pmm.mem_top
		   << ",fr1b=" << (void *)si->pmm.free1_base
		   << ",fr1t=" << (void *)si->pmm.free1_top
		   << ",fr2b=" << (void *)si->pmm.free2_base
		   << ",fr2t=" << (void *)si->pmm.free2_top
		   << "}"
		   << ",code_size=" << MMU::pages(si->lm.sys_code_size)
		   << ",data_size=" << MMU::pages(si->lm.sys_data_size)
		   << ",stack_size=" << MMU::pages(si->lm.sys_stack_size)
		   << ")\n";

    // Get the physical address for the System Page Table
    PT_Entry * sys_pt = reinterpret_cast<PT_Entry *>((void *)si->pmm.sys_pt);

    // Clear the System Page Table
//    memset(sys_pt, 0, MMU::PT_ENTRIES);

    // IDT
    sys_pt[MMU::page(IDT)] = si->pmm.idt | Flags::SYS;

    // GDT
    sys_pt[MMU::page(GDT)] = si->pmm.gdt | Flags::SYS;

    // Set an entry to this page table, so the system can access it later
    sys_pt[MMU::page(SYS_PT)] = si->pmm.sys_pt | Flags::SYS;

    // System Page Directory
    sys_pt[MMU::page(SYS_PD)] = si->pmm.sys_pd | Flags::SYS;

    // System Info
    sys_pt[MMU::page(SYS_INFO)] = si->pmm.sys_info | Flags::SYS;

    unsigned int i;
    PT_Entry aux;

    // OS code
    for(i = 0, aux = si->pmm.sys_code;
	i < MMU::pages(si->lm.sys_code_size);
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(SYS_CODE) + i] = aux | Flags::SYS;

    // OS data
    for(i = 0, aux = si->pmm.sys_data;
	i < MMU::pages(si->lm.sys_data_size);
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(SYS_DATA) + i] = aux | Flags::SYS;

    // OS stack (who needs a stack?)
    for(i = 0, aux = si->pmm.sys_stack;
	i < MMU::pages(si->lm.sys_stack_size);
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(SYS_STACK) + i] = aux | Flags::SYS;

    db<Setup>(INF) << "SPT=" << *((Page_Table *)sys_pt) << "\n";
}

//========================================================================
void PC_Setup::setup_sys_pd()
{
    db<Setup>(TRC) << "setup_sys_pd(pmm={idt=" << (void *)si->pmm.idt
		   << ",...},mem_base=" << (void *)si->pmm.mem_base
		   << ",mem_top=" << (void *)si->pmm.mem_top
		   << ",pci_base=" << (void *)si->pmm.io_mem_base
		   << ",pci_top=" << (void *)si->pmm.io_mem_top
		   << ")\n";

    // Get the physical address for the System Page Directory
    PT_Entry * sys_pd = reinterpret_cast<PT_Entry *>((void *)si->pmm.sys_pd);

    // Clear the System Page Directory
//    memset(sys_pd, 0, MMU::PT_ENTRIES);

    // Calculate the number of page tables needed to map the physical memory
    unsigned int mem_size = MMU::pages(si->bm.mem_top - si->bm.mem_base);
    int n_pts = (mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;

    // Map all physical memory into the page tables pointed by phy_mem_pts
    // These will be attached at both PHY_MEM and MEM_BASE thus flags
    // must consider application access
    PT_Entry * pts = reinterpret_cast<PT_Entry *>((void *)si->pmm.phy_mem_pts);
    for(unsigned int i = MMU::pages(si->pmm.mem_base); i < mem_size; i++)
	pts[i] = (i * sizeof(Page)) | Flags::APP;

    // Attach all physical memory starting at PHY_MEM
    for(int i = 0; i < n_pts; i++)
	sys_pd[MMU::directory(PHY_MEM) + i] =
	    (si->pmm.phy_mem_pts + i * sizeof(Page)) | Flags::SYS;

    // Attach memory starting at MEM_BASE
    for(unsigned int i = MMU::directory(
	    MMU::align_directory(si->pmm.mem_base));
	i < MMU::directory(MMU::align_directory(si->pmm.mem_top));
	i++)
	sys_pd[i] = (si->pmm.phy_mem_pts + i * sizeof(Page)) | Flags::APP;

    // Calculate the number of page tables needed to map the PCI AS
    unsigned int io_mem_size =
	MMU::pages(si->pmm.io_mem_top - si->pmm.io_mem_base);
    n_pts = (io_mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;

    // Map PCI addres space into the page tables pointed by io_mem_pts
    pts = reinterpret_cast<PT_Entry *>((void *)si->pmm.io_mem_pts);
    for(unsigned int i = 0; i < io_mem_size; i++)
	pts[i] = (si->pmm.io_mem_base + i * sizeof(Page)) | Flags::PCI;

    // Attach PCI devices' memory to IO_MEM
    for(int i = 0; i < n_pts; i++)
	sys_pd[MMU::directory(IO_MEM) + i] =
	    (si->pmm.io_mem_pts + i * sizeof(Page)) | Flags::PCI;

    // Map the system 4M logical address space at the top of the 4Gbytes
    sys_pd[MMU::directory(SYS_CODE)] = si->pmm.sys_pt | Flags::SYS;

    db<Setup>(INF) << "SPD=" << *((Page_Directory *)sys_pd) << "\n";
}

//========================================================================
void PC_Setup::load_parts()
{
    // Relocate System_Info
    if(sizeof(System_Info<PC>) > sizeof(Page))
	db<Setup>(WRN) << "System_Info is bigger than a page ("
		       << sizeof(System_Info<PC>) << ")!\n";
    memcpy(reinterpret_cast<void *>(SYS_INFO), bi, sizeof(System_Info<PC>));
    
    // Load INIT
    if(si->lm.has_ini) {
	db<Setup>(TRC) << "PC_Setup::load_init()\n";
	ELF * ini_elf = reinterpret_cast<ELF *>(&bi[si->bm.init_offset]);
	if(ini_elf->load_segment(0) < 0) {
	    db<Setup>(ERR)
		<< "INIT code segment was corrupted during SETUP!\n";
	    panic();
	}
	for(int i = 1; i < ini_elf->segments(); i++)
	    if(ini_elf->load_segment(i) < 0) {
		db<Setup>(ERR)
		    << "INIT data segment was corrupted during SETUP!\n";
		panic();
	    }
    }

    // Load SYSTEM
    if(si->lm.has_sys) {
	db<Setup>(TRC) << "PC_Setup::load_os()\n";
	ELF * sys_elf = reinterpret_cast<ELF *>(&bi[si->bm.system_offset]);
	if(sys_elf->load_segment(0) < 0) {
	    db<Setup>(ERR)
		<< "OS code segment was corrupted during SETUP!\n";
	    panic();
	}
	for(int i = 1; i < sys_elf->segments(); i++)
	    if(sys_elf->load_segment(i) < 0) {
		db<Setup>(ERR)
		    << "OS data segment was corrupted during SETUP!\n";
		panic();
	    }
    }

    // Load APP
    if(si->lm.has_app) {
	ELF * app_elf = reinterpret_cast<ELF *>(&bi[si->bm.application_offset]);
	db<Setup>(TRC) << "PC_Setup::load_app()\n";
	if(app_elf->load_segment(0) < 0) {
	    db<Setup>(ERR)
		<< "Application code segment was corrupted during SETUP!\n";
	    panic();
	}
	for(int i = 1; i < app_elf->segments(); i++)
	    if(app_elf->load_segment(i) < 0) {
		db<Setup>(ERR) << 
		    "Application data segment was corrupted during SETUP!\n";
		panic();
	    }
    }
}

//========================================================================
void PC_Setup::call_next()
{
    // Check for next stage
    Log_Addr ip;
    if(si->lm.has_ini) {
	db<Setup>(TRC) << "Executing system global constructors ...\n";
	reinterpret_cast<void (*)()>((void *)si->lm.sys_entry)();
	ip = si->lm.ini_entry;
    } else if(si->lm.has_sys)
	ip = si->lm.sys_entry;
    else
	ip = si->lm.app_entry;

    db<Setup>(TRC) << "PC_Setup::call_next(ip=" << ip
		   << ",sp=" << (void *)(SYS_STACK + SYS_STACK_SIZE
					 - 2 * sizeof(int))
		   << ") => ";
    if(si->lm.has_ini)
	db<Setup>(TRC) << "INIT\n";
    else if(si->lm.has_sys)
	db<Setup>(TRC) << "SYSTEM\n";
    else
	db<Setup>(TRC) << "APPLICATION\n";

    db<Setup>(INF) << "PC_Setup ends here!\n\n";

    // The 2 integers on the stack are ???
    ASM("movl	%0, %%esp	\n"
 	"call	*%%ebx		\n"
 	: : "i"(SYS_STACK + SYS_STACK_SIZE - 2 * sizeof(int)),
	    "b"(static_cast<unsigned int>(ip)));
}

//========================================================================
void PC_Setup::pci_aperture(unsigned int * base, unsigned int * top)
{
    db<Setup>(TRC) << "PC_Setup::pci_aperture()\n";

    // Scan the PCI bus looking for devices with memory mapped regions
    *base = ~0U;
    *top = 0U;
    for(int bus = 0; bus <= Traits<PCI>::MAX_BUS; bus++) {
	for(int dev_fn = 0; dev_fn <= Traits<PCI>::MAX_DEV_FN; dev_fn++) {
	    PCI::Locator loc(bus, dev_fn);
	    PCI::Header hdr;
 	    PCI::header(loc, &hdr);
	    if(hdr) {
		db<Setup>(INF) << "PCI" << hdr << "\n";
		for(unsigned int i = 0; i < PCI::Region::N; i++) {
		    PCI::Region * reg = &hdr.region[i];
		    if(*reg) {
			db<Setup>(INF)  << "  reg[" << i << "]="
					<< *reg << "\n";
			if(reg->memory) {
			    if(reg->phy_addr < *base)
				*base = reg->phy_addr;
			    if((reg->phy_addr + reg->size) > *top)
				*top = reg->phy_addr + reg->size;
			}
		    }
		}
	    }
	}
    }

    db<Setup>(INF) << "PCI address space={base=" << (void *)*base
		   << ",top=" << (void *)*top << "}\n";
}

__END_SYS

__USING_SYS

extern "C" { void _start(); }
extern "C" { void setup(char * bi); }

//========================================================================
// _start	          
//
// In order to support larger boot images, PC_BOOT uses all memory
// below 640 Kb. In order to have more freedom to setup the system,
// we move PC_SETUP to a more convenient location.
// "_start" MUST BE PC_SETUP's first function, since PC_BOOT assumes 
// offset 0 to be the entry point.
// The initial stack pointer is inherited from PC_BOOT (i.e.,
// somewhere below 0x7c00).
// We can't "kout" here because the data segment is unreachable
// and "kout" has static data.
// THIS FUNCTION MUST BE RELOCATABLE, because it won't run at the
// address it has been compiled for.
//------------------------------------------------------------------------
void _start()
{
    // Set EFLAGS (disable interrupts)
    CPU::flags(CPU::flags() & CPU::FLAG_CLEAR);

    // The boot strap loaded the boot image at BOOT_IMAGE_ADDR
    char * bi = reinterpret_cast<char *>(Traits<PC>::BOOT_IMAGE_ADDR);

    // Get the System_Info  (first thing in the boot image)
    System_Info<PC> * si = reinterpret_cast<System_Info<PC> *>(bi);

    // Check SETUP integrity and get information about its ELF structure
    ELF * elf = reinterpret_cast<ELF *>(&bi[si->bm.setup_offset]);
    if(!elf->valid())
 	Machine::panic();
    char * entry = reinterpret_cast<char *>(elf->entry());

    // Test if we can access the address for which SETUP has been compiled
    *entry = 'G';
    if(*entry != 'G')
	Machine::panic();

    // Load SETUP considering the address in the ELF header
    // Check if this wouldn't destroy the boot image
    char * addr = reinterpret_cast<char *>(elf->segment_address(0));
    int size = elf->segment_size(0);
    if(addr <= &bi[si->bm.img_size])
	Machine::panic();
    if(elf->load_segment(0) < 0)
	Machine::panic();

    // Move the boot image to after SETUP, so there will be nothing else
    // below SETUP to be preserved
    // SETUP code + data + stack)
    register char * dst =
	MMU::align_page(entry + size + sizeof(MMU::Page));
    memcpy(dst, bi, si->bm.img_size);

    // Setup a single page stack for SETUP after its data segment
    // SP = "entry" + "size" + sizeof(Page)
    // Be carefull: we'll lost our stack, so everything must be in regs!
    ASM("movl %0, %%esp" : : "r" (dst));

    // Pass the boot image to SETUP
    ASM("pushl %0" : : "r" (dst));

    // Call setup()
    // the assembly is necessary because the compiler generates
    // relative calls and we need an absolute one
    ASM("call *%0" : : "r" (&setup));
}

void setup(char * bi) 
{
    PC_Setup pc_setup(bi);
}
