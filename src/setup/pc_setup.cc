// PC_SETUP
//
// SETUP is responsible for bringing the machine into a usable state. It
// sets up several IA32 dependent data structures (IDT, GDT, TSSs, etc), a
// basic memory model (flat), a basic thread model (exclusive task/exclusive
// thread), the FPU and other selected devices. Note that setup deals with
// hardware dependent initialization. OS initialization is due to PC_INIT.

#include <utility/elf.h>
#include <utility/string.h>
#include <utility/ostream.h>
#include <utility/debug.h>
#include <mach/pc/pc.h>

__USING_SYS

// IA32 Imports
typedef CPU::Reg8 Reg8;
typedef CPU::Reg16 Reg16;
typedef CPU::Reg32 Reg32;
typedef CPU::Reg64 Reg64;
typedef CPU::Phy_Addr Phy_Addr;
typedef CPU::Log_Addr Log_Addr;
typedef MMU::Page Page;
typedef MMU::Page_Table Page_Table;
typedef MMU::Page_Directory Page_Directory;
typedef MMU::PT_Entry PT_Entry;
typedef MMU::PD_Entry PD_Entry;
typedef MMU::IA32_Flags Flags;

// System_Info Imports
typedef System_Info::Physical_Memory_Map PMM;
typedef System_Info::Logical_Memory_Map LMM;
typedef System_Info::Boot_Map BM;
typedef Memory_Map<PC> MM;
typedef Traits<PC> TR;

// Prototypes
extern "C" { void _start(); }
int  main(char *, unsigned int, char *);
void setup_pci(Phy_Addr *, unsigned int *);
void setup_gdt(Phy_Addr);
void setup_idt(Phy_Addr);
void setup_sys_pt(PMM *, int, int, int);
void setup_sys_pd(PMM *, unsigned int, Phy_Addr, unsigned int);
void setup_lmm(LMM *, Log_Addr, Log_Addr);
void copy_sys_info(System_Info *, System_Info *);
void call_next(Log_Addr);
void page_fault(Reg32, Reg32, Reg32, Reg32, Reg32, Reg32);
void gpf(Reg32, Reg32, Reg32, Reg32, Reg32, Reg32);
void fpu();
void syscall();
void hard_int();
void panic();

__BEGIN_SYS

OStream kout, kerr;
bool has_init;
bool has_system;

__END_SYS

ASM("jmp _start");

//========================================================================
// _start	          
//
// Desc: In order to support larger boot images, PC_BOOT uses all memory
//	 below 640 Kb. In order to have more freedom to setup the system,
//	 we move PC_SETUP to a more convenient location.
//	 "_start" MUST BE PC_SETUP's entry point, so, if your compiler
//	 doesn't assume "_start" to be the entry point (GCC does), you
//	 somehow have to arrange for this.
//	 The initial stack pointer is inherited from PC_BOOT (i.e.,
//	 somewhere below 0x7c00).
//	 We can't "kout" here because the data segment is unreachable
//	 and "kout" has static data.
//	 THIS FUNCTION MUST BE RELOCATABLE, because it won't run at the
//	 address it has been compiled for.
//------------------------------------------------------------------------
void _start()
{
    // Set EFLAGS
    CPU::flags(CPU::flags() & CPU::FLAG_CLEAR);

    // The boot strap loaded the boot image at BOOT_IMAGE_ADDR
    char * bi = reinterpret_cast<char *>(TR::BOOT_IMAGE_ADDR);

    // Get the System_Info  (first thing in the boot image)
    System_Info * si = reinterpret_cast<System_Info *>(bi);

    // Check SETUP integrity and get information about its ELF structure
    ELF * elf = reinterpret_cast<ELF *>(&bi[si->bm.setup_off]);
    if(!elf->valid())
 	panic();
    char * entry = reinterpret_cast<char *>(elf->entry());
    if(elf->segments() != 1)
 	panic();

    // Test if we can access the address for which SETUP has been compiled
    *entry = 'G';
    if(*entry != 'G')
	panic();

    // Load SETUP considering the address in the ELF header
    // Check if this wouldn't destroy the boot image
    register char * addr = reinterpret_cast<char *>(elf->segment_address(0));
    register int size = elf->segment_size(0);
    if(addr <= &bi[si->bm.img_size])
	panic();
    if(elf->load_segment(0) < 0)
	panic();

    // Move the boot image to after SETUP, so there will be nothing else
    // below SETUP to be preserved
    // SETUP code + data + stack)
    register char * dst = entry + size + sizeof(Page);
    memcpy(dst, bi, si->bm.img_size);

    // Setup a single page stack for SETUP after its data segment
    // SP = "entry" + "size" + sizeof(Page)
    // Be carefull: we'll lost our stack, so everything must be in regs!
    ASM("movl %0, %%esp" : : "r" (dst));

    // Pass the boot image to SETUP
    ASM("pushl %0" : : "r" (dst));

    // Pass SETUP its size
    ASM("pushl %0" : : "r" (size));

    // Pass SETUP its loading address
    ASM("pushl %0" : : "r" (addr));

    // Call main() (the assembly is necessary because the compiler generates
    // relative calls and we need an absolute)
    ASM("call *%0" : : "r" (&main));
}

//========================================================================
// main
//
// Desc: We have much to do here, you'd better take a look at the remarks
//	 below. Important for now is that we've got a single-page stack
//	 and we don't check for overflows, so be careful!
//
// Parm: setup_addr -> a pointer to PC_SETUP's loaded image
//	 setup_size -> PC_SETUP's size in memory
//	 bi -> a pointer to the image loaded from disk by PC_BOOT
//------------------------------------------------------------------------
int main(char * setup_addr, unsigned int setup_size, char * bi)
{
    ELF * elf;

    db<Setup>(TRC) << "PC_Setup(stp=" << (void *)setup_addr
		   << ",stp_sz=" << setup_size
		   << ",bi=" << (void *)bi << ")\n";

    // System_Info is the first thing in the boot image
    System_Info * si = reinterpret_cast<System_Info *>(bi);
    has_init = (si->bm.init_off != -1);
    has_system = (si->bm.system_off != -1);
    if(!has_system)
	db<Setup>(WRN) << "No SYSTEM in boot image, assuming EPOS is a library!\n";

    // Check INIT integrity and get the size of its code+data segment
    Log_Addr init_entry = 0;
    unsigned int init_segments = 0;
    unsigned int init_size = 0;
    if(has_init) {
	elf = reinterpret_cast<ELF *>(&bi[si->bm.init_off]);
	if(!elf->valid()) {
	    db<Setup>(ERR) << "INIT ELF image is corrupted!\n";
	    panic();
	}
	init_entry = elf->entry();
	init_segments = elf->segments();
	init_size = elf->segment_size(0);
 	if(init_segments > 1) {
 	    db<Setup>(ERR) << "INIT ELF image has more than one segment ("
 			   << elf->segments() << ")!\n";
	    panic();
	}
    }

    // Check OS integrity and get the size of its code and data segments
    Log_Addr sys_entry = 0;
    unsigned int sys_segments = 0;
    Log_Addr sys_code = 0;
    unsigned int sys_code_size = 0;
    Log_Addr sys_data = 0;
    unsigned int sys_data_size = 0;
    Log_Addr sys_stack = MM::SYS_STACK;
    unsigned int sys_stack_size = TR::SYSTEM_STACK_SIZE;
    if(has_system) {
	elf = reinterpret_cast<ELF *>(&bi[si->bm.system_off]);
	if(!elf->valid()) {
	    db<Setup>(ERR) << "OS ELF image is corrupted!\n";
	    panic();
	}
	sys_entry = elf->entry();
	sys_segments = elf->segments();
	sys_code = elf->segment_address(0);
	sys_code_size = elf->segment_size(0);
	sys_data = elf->segment_address(1);
	for(unsigned int i = 1; i < sys_segments; i++) {
	    if(elf->segment_address(i) < sys_data)
		sys_data = elf->segment_address(i);
	    sys_data_size += elf->segment_size(i);
	}
	if(sys_code != MM::SYS_CODE) {
	    db<Setup>(ERR) << "OS code segment address do not match "
			   << "the machine's memory map!\n";
	    panic();
	}
	if(sys_code + sys_code_size > sys_data) {
	    db<Setup>(ERR) << "OS code segment is too large!\n";
	    panic();
	}

	if(sys_data != MM::SYS_DATA) {
	    db<Setup>(ERR) << "OS code segment address do not match "
			   << "the machine's memory map!\n";
	    panic();
	}

	if(sys_data + sys_data_size > sys_stack) {
	    db<Setup>(ERR) << "OS data segment is too large!\n";
	    panic();
	}
	if(sys_data + sys_data_size > sys_stack) {
	    db<Setup>(ERR) << "OS data segment is too large!\n";
	    panic();
	}
	if(MMU::page_tables(MMU::pages(sys_stack - MM::SYS + sys_stack_size))
	   > 1) {
	    db<Setup>(ERR) << "OS stack segment is too large!\n";
	    panic();
	}
    }

    // Check APP integrity and get the size of all its segments
    elf = reinterpret_cast<ELF *>(&bi[si->bm.loader_off]);
    if(!elf->valid()) {
        db<Setup>(ERR) << "Application ELF image is corrupted!\n";
        panic();
    }
    Log_Addr app_entry = elf->entry();
    unsigned int app_segments = elf->segments();
    Log_Addr app_code = elf->segment_address(0);
    unsigned int app_code_size = elf->segment_size(0);
    Log_Addr app_data = elf->segment_address(1);
    unsigned int app_data_size = 0;
    for(unsigned int i = 1; i < app_segments; i++) {
	if(elf->segment_address(i) < app_data)
	    app_data = elf->segment_address(i);
	app_data_size += elf->segment_size(i);
    }

    // If we didn't get our node's id in the boot image, we'll to try to
    // get if from an eventual BOOPT reply used to boot up the system before
    // we allocate more memory
    // if(si->bm.host_id == (unsigned short) -1)
    // get_bootp_info(&si->bm.host_id);

    // Say hi! :-)
    kout << "Setting up this machine as follows: \n";
    kout << "  Processor: IA32\n";
    kout << "  Memory:    " << si->bm.mem_size/1024 << " Kbytes\n";
    kout << "  Node Id:   ";
    if(si->bm.node_id != -1)
	kout << si->bm.node_id << " (" << si->bm.n_nodes << ")\n";
    else
	kout << "will get from the network!\n";
    kout << "  Setup:     " << setup_size << " bytes\n";
    kout << "  Init:      " << init_size << " bytes\n";
    kout << "  OS code:   " << sys_code_size << " bytes";
    kout << "\tdata: " << sys_data_size << " bytes";
    kout << "\tstack: " << sys_stack_size << " bytes\n";
    kout << "  APP code:  " << app_code_size << " bytes";
    kout << "\tdata: " << app_data_size << " bytes\n";

    // Setup the interrupt controller
    // The BIOS sets hardware interrupts to 0x08-0x0f, but these IDT
    // entries are assigned to internal exceptions in the i[x>1]86
    // We'll remap interrupts to HARD_INT and then disable them
    IC::init(si);

    // Setup the PCI bus controller
    setup_pci(reinterpret_cast<Phy_Addr *>(&si->pmm.io_mem),
	      &si->pmm.io_mem_size);

    // Align and convert the following sizes to pages
    sys_code_size = MMU::pages(sys_code_size);
    sys_data_size = MMU::pages(sys_data_size);
    sys_stack_size = MMU::pages(sys_stack_size);
    app_code_size = MMU::pages(app_code_size);
    app_data_size = MMU::pages(app_data_size);
    si->mem_size  = MMU::pages(si->bm.mem_size);
    si->mem_free = si->mem_size;

    // Allocate (reserve) memory for all entities we have to setup.
    // We'll start at the highest address to make possible a memory model
    // on which the application's logical and physical address spaces match.

    // IDT (1 x sizeof(Page))
    si->mem_free -= 1;
    si->pmm.int_vec = si->mem_free * sizeof(Page);

    // GDT (1 x sizeof(Page))
    si->mem_free -= 1;
    si->pmm.mach1 = si->mem_free * sizeof(Page);

    // System Page Table (1 x sizeof(Page))
    si->mem_free -= 1;
    si->pmm.sys_pt = si->mem_free * sizeof(Page);

    // System Page Directory (1 x sizeof(Page))
    si->mem_free -= 1;
    si->pmm.sys_pd = si->mem_free * sizeof(Page);

    // System Info (1 x sizeof(Page))
    si->mem_free -= 1;
    si->pmm.sys_info = si->mem_free * sizeof(Page);

    // Page tables to map the whole physical memory
    // = NP/NPTE_PT * sizeof(Page)
    //   NP = size of physical memory in pages
    //   NPTE_PT = number of page table entries per page table
    si->mem_free -= (si->mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;
    si->pmm.phy_mem_pts = si->mem_free * sizeof(Page);

    // Page tables to map the IO address space
    // = NP/NPTE_PT * sizeof(Page)
    // NP = size of PCI address space in pages
    // NPTE_PT = number of page table entries per page table
    si->mem_free -=
	(si->pmm.io_mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;
    si->pmm.io_mem_pts = si->mem_free * sizeof(Page);

    // OS code segment (in pages)
    si->mem_free -= sys_code_size;
    si->pmm.sys_code = si->mem_free * sizeof(Page);

    // OS data segment (in pages)
    si->mem_free -= sys_data_size;
    si->pmm.sys_data = si->mem_free * sizeof(Page);

    // OS stack segment (in pages, from Traits)
    si->mem_free -= sys_stack_size;
    si->pmm.sys_stack = si->mem_free * sizeof(Page);

    // All memory bolow this is free to applications
    si->pmm.app_lo = si->bm.mem_base;
    si->pmm.app_hi = si->mem_free * sizeof(Page);
    si->pmm.mach2 = app_code + app_code_size * sizeof(Page);
    si->pmm.mach3 = MMU::pages(app_data - app_code) - app_code_size;
    si->pmm.free = app_data + app_data_size * sizeof(Page);
    si->pmm.free_size = MMU::pages(si->pmm.app_hi - app_data) - app_data_size;
    if(si->bm.app_off != -1) {
	si->pmm.img = Phy_Addr(&bi[si->bm.app_off]);
	si->pmm.img_size = si->bm.img_size - si->bm.app_off;
    }

    // Test if we didn't overlap SETUP and the boot image
    if(si->mem_free * sizeof(Page) <=
       reinterpret_cast<unsigned int>(setup_addr) + setup_size) {
	db<Setup>(ERR) << "SETUP would have been overwritten!\n";
	panic();
    }

    // Zero the memory allocated to the system
    memset(reinterpret_cast<void *>(si->mem_free * sizeof(Page)), 0,
	   (si->mem_size - si->mem_free) * sizeof(Page));

    // Setup the IDT
    setup_idt(si->pmm.int_vec);

    // Setup the GDT
    setup_gdt(si->pmm.mach1);

    // Setup the System Page Table
    setup_sys_pt(&si->pmm, sys_code_size, sys_data_size, sys_stack_size);

    // Setup the System Page Directory and map physical memory
    setup_sys_pd(&si->pmm, si->mem_size, si->pmm.io_mem, si->pmm.io_mem_size);

    // Set IDTR (limit = 1 x sizeof(Page))
    CPU::idtr(sizeof(Page) - 1, MM::INT_VEC);

    // Reload GDTR with its linear address (one more absurd from Intel!)
    CPU::gdtr(sizeof(Page) - 1, MM::MACH1);

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
    ASM("ljmp %0, %1 + next" : : "i"(CPU::SEL_FLT_CODE), "i"(MM::PHY_MEM));
    ASM("next:");

    // Reload segment registers with GDT_FLT_DATA
    ASM("" : : "a" (CPU::SEL_FLT_DATA));
    ASM("movw %ax, %ds");
    ASM("movw %ax, %es");
    ASM("movw %ax, %fs");
    ASM("movw %ax, %gs");
    ASM("movw %ax, %ss");

    // Set stack pointer to its logical address
    ASM("orl %0, %%esp" : : "i" (MM::PHY_MEM));

    // Adjust pointers that will still be used to their logical addresses
    bi = reinterpret_cast<char *>((unsigned int)(bi) | MM::PHY_MEM);

    // Flush TLB to ensure we've got the right memory organization
    MMU::flush_tlb();

    // Load INIT
    if(has_init) {
	db<Setup>(TRC) << "PC_Setup::load_init()\n";
	elf = reinterpret_cast<ELF *>(&bi[si->bm.init_off]);
	if(elf->load_segment(0) < 0) {
	    db<Setup>(ERR)
		<< "INIT code+data segment was corrupted during SETUP!\n";
	    panic();
	}
    }

    // Load OS
    if(has_system) {
	db<Setup>(TRC) << "PC_Setup::load_os()\n";
	elf = reinterpret_cast<ELF *>(&bi[si->bm.system_off]);
	if(elf->load_segment(0) < 0) {
	    db<Setup>(ERR)
		<< "OS code segment was corrupted during SETUP!\n";
	    panic();
	}
	for(unsigned int i = 1; i < sys_segments; i++)
	    if(elf->load_segment(i) < 0) {
		db<Setup>(ERR)
		    << "OS data segment was corrupted during SETUP!\n";
		panic();
	    }
    }

    // Load APP
    elf = reinterpret_cast<ELF *>(&bi[si->bm.loader_off]);
    db<Setup>(TRC) << "PC_Setup::load_app()\n";
    if(elf->load_segment(0) < 0) {
        db<Setup>(ERR)
	    << "Application code segment was corrupted during SETUP!\n";
        panic();
    }
    for(unsigned int i = 1; i < app_segments; i++)
        if(elf->load_segment(i) < 0) {
           db<Setup>(ERR)
	       << "Application data segment was corrupted during SETUP!\n";
           panic();
        }

    // Copy System_Info
    si = reinterpret_cast<System_Info *>(MM::SYS_INFO);
    copy_sys_info(reinterpret_cast<System_Info *>(bi), si);

    // Setup Logical_Memory_Map
    setup_lmm(&si->lmm, app_entry, si->pmm.app_hi);

    // Startup the FPU
    // CPU::init_fpu();

    // SETUP ends here
    if(has_init) {
	db<Setup>(TRC) << "Executing system global constructors ...\n";
	static_cast<void (*)()>(sys_entry)();
	call_next(init_entry);
    } else if(has_system)
	call_next(sys_entry);
    else
	call_next(app_entry);

    // SETUP is now part of the free memory and this point should never be
    // reached, but, just for ... :-)
    panic();

    // Just to avoid the warning
    return -1;
}

//========================================================================
// setup_pci
//
// Desc: Calculate the PCI address space apperture.
//
// Parm: addr <- PCI address space base
//       size <- PCI address space size (in pages)
//	 si   <- IO map
//------------------------------------------------------------------------
void setup_pci(Phy_Addr * addr, unsigned int * size)
{
    db<Setup>(TRC) << "setup_pci()\n";

    // Scan the PCI bus looking for devices with memory mapped regions
    Phy_Addr base = ~0U;
    Phy_Addr top = (void *)0;
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
			    if(reg->phy_addr < base)
				base = reg->phy_addr;
			    if((reg->phy_addr + reg->size) > top)
				top = reg->phy_addr + reg->size;
			}
		    }
		}
	    }
	}
    }
    *addr = base;
    *size = MMU::pages(top - base);

    db<Setup>(INF) << "PCI address space={base=" << *addr
		   << ",size=" << (void *)(*size * sizeof(Page)) << "}\n";
}

//========================================================================
// setup_idt
//
// Desc: Setup the IDT with panic for all entries but GPF and page-fault.
//
// Parm: addr -> IDT PHYSICAL address
//------------------------------------------------------------------------
void setup_idt(Phy_Addr addr)
{
    typedef CPU::IDT_Entry IDT_Entry;

    db<Setup>(TRC) << "setup_idt(idt=" << (void *)addr << ")\n";

    // Adjust handler addresses to logical addresses
    Log_Addr panic_h = Log_Addr(&panic) | MM::PHY_MEM;
    Log_Addr gpf_h = Log_Addr(&gpf) | MM::PHY_MEM;
    Log_Addr page_fault_h = Log_Addr(&page_fault) | MM::PHY_MEM;
    Log_Addr fpu_h = Log_Addr(&fpu) | MM::PHY_MEM;
    Log_Addr syscall_h = Log_Addr(&syscall) | MM::PHY_MEM;
    Log_Addr hard_int_h = Log_Addr(&hard_int) | MM::PHY_MEM;

    // Map all handlers to panic()
    IDT_Entry * idt = reinterpret_cast<IDT_Entry *>((void *)addr);
    for(unsigned int i = 0; i < CPU::IDT_ENTRIES; i++)
	idt[i] = IDT_Entry(CPU::GDT_SYS_CODE, panic_h, CPU::SEG_IDT_ENTRY);

    // Catch GPF, PAGE_FAULT and FPU
    idt[CPU::EXC_GPF] = IDT_Entry(CPU::GDT_SYS_CODE, gpf_h,
				  CPU::SEG_IDT_ENTRY);
    idt[CPU::EXC_PF] = IDT_Entry(CPU::GDT_SYS_CODE, page_fault_h,
				 CPU::SEG_IDT_ENTRY);
    idt[CPU::EXC_NODEV] = IDT_Entry(CPU::GDT_SYS_CODE, fpu_h,
				    CPU::SEG_IDT_ENTRY);

    // Catch system calls generated by accident
    idt[Traits<PC>::SYSCALL_INT] = IDT_Entry(CPU::GDT_SYS_CODE, syscall_h,
					     CPU::SEG_IDT_ENTRY);

    // Catch hardware interrupts
    for(unsigned int i = TR::HARDWARE_INT_OFFSET;
	i < TR::HARDWARE_INT_OFFSET + PC_IC::IRQS;
	i++)
	idt[i] = IDT_Entry(CPU::GDT_SYS_CODE, hard_int_h,
			   CPU::SEG_IDT_ENTRY);

    db<Setup>(INF) << "IDT[0  ]=" << idt[0]
		   << " (" << panic_h << ")\n";
    db<Setup>(INF) << "IDT[GPF=" << CPU::EXC_GPF << "]="
		   << idt[CPU::EXC_GPF] << " (" << gpf_h << ")\n";
    db<Setup>(INF) << "IDT[PF =" << CPU::EXC_PF << "]="
		   << idt[CPU::EXC_PF] << " (" << page_fault_h << ")\n";
    db<Setup>(INF) << "IDT[SYS=" << Traits<PC>::SYSCALL_INT << "]="
		   << idt[Traits<PC>::SYSCALL_INT]
		   << " (" << syscall_h << ")\n";
}

//========================================================================
// setup_gdt
//
// Desc: Setup the GDT, including entries for application and system
//	 code, data and stack.
//
// Parm: addr       -> GDT PHYSICAL address
//------------------------------------------------------------------------
void setup_gdt(Phy_Addr addr)
{
    typedef CPU::GDT_Entry GDT_Entry;

    db<Setup>(TRC) << "setup_gdt(gdt=" << (void *)addr << ")\n";

    GDT_Entry * gdt = (GDT_Entry *)(unsigned int)addr;

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
// setup_sys_pt
//
// Desc: Setup the System Page Table
//
// Parm: pmm            -> physical memory map
//	 sys_code_size  -> system code size in Ix86_Pages
//	 sys_data_size  -> system data size in Ix86_Pages
//	 sys_stack_size -> system stack size in pages
//------------------------------------------------------------------------
void setup_sys_pt(PMM * pmm, int sys_code_size, int sys_data_size,
		  int sys_stack_size)
{
    db<Setup>(TRC) << "setup_sys_pt(pmm={idt=" << (void *)pmm->int_vec
		   << ",gdt="  << (void *)pmm->mach1
		   << ",pt="   << (void *)pmm->sys_pt
		   << ",pd="   << (void *)pmm->sys_pd
		   << ",info=" << (void *)pmm->sys_info
		   << ",mem="  << (void *)pmm->phy_mem_pts
		   << ",io="   << (void *)pmm->io_mem_pts
		   << ",sysc=" << (void *)pmm->sys_code
		   << ",sysd=" << (void *)pmm->sys_data
		   << ",syss=" << (void *)pmm->sys_stack
		   << ",apl="  << (void *)pmm->app_lo
		   << ",aph="  << (void *)pmm->app_hi
		   << ",fr1b=" << (void *)pmm->mach2
		   << ",fr1s=" << (void *)pmm->mach3
		   << ",fr2b=" << (void *)pmm->free
		   << ",fr2s=" << (void *)pmm->free_size
		   << "}"
		   << ",code_size=" << sys_code_size
		   << ",data_size=" << sys_data_size
		   << ",stack_size=" << sys_stack_size << ")\n";

    // Get the physical address for the System Page Table
    PT_Entry * sys_pt = reinterpret_cast<PT_Entry *>(pmm->sys_pt);

    // Clear the System Page Table
    memset(sys_pt, 0, MMU::PT_ENTRIES);

    // IDT
    sys_pt[MMU::page(MM::INT_VEC)] = pmm->int_vec | Flags::SYS;

    // GDT
    sys_pt[MMU::page(MM::MACH1)] = pmm->mach1 | Flags::SYS;

    // Set an entry to this page table, so the system can access it later
    sys_pt[MMU::page(MM::SYS_PT)] = pmm->sys_pt | Flags::SYS;

    // System Page Directory
    sys_pt[MMU::page(MM::SYS_PD)] = pmm->sys_pd | Flags::SYS;

    // System Info
    sys_pt[MMU::page(MM::SYS_INFO)] = pmm->sys_info | Flags::SYS;

    int i;
    PT_Entry aux;

    // OS code
    for(i = 0, aux = pmm->sys_code;
	i < sys_code_size;
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(MM::SYS_CODE) + i] = aux | Flags::SYS;

    // OS data
    for(i = 0, aux = pmm->sys_data;
	i < sys_data_size;
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(MM::SYS_DATA) + i] = aux | Flags::SYS;

    // OS stack (who needs a stack?)
    for(i = 0, aux = pmm->sys_stack;
	i < sys_stack_size;
	i++, aux = aux + sizeof(Page))
	sys_pt[MMU::page(MM::SYS_STACK) + i] = aux | Flags::SYS;

    db<Setup>(INF) << "SPT=" << *((Page_Table *)sys_pt) << "\n";
}

//========================================================================
// setup_sys_pd
//
// Desc: Setup the System Page Directory and maps the whole physical
//	 memory at both MM::PHY_MEM and "pmm->app_lo".
//
// Parm: pmm	          -> physical memory map
// 	 phy_mem_size     -> size of physical memory in Pages
// 	 io_mem_phy_addr  -> PCI address space physical address
// 	 io_mem_size      -> size of PCI address space in Pages
//------------------------------------------------------------------------
void setup_sys_pd(PMM * pmm, unsigned int phy_mem_size,
		  Phy_Addr io_mem_phy_addr, unsigned int io_mem_size)
{
    int n_pts;
    PT_Entry *pts, *sys_pd;

    db<Setup>(TRC) << "setup_sys_pd(pmm={idt=" << (void *)pmm->int_vec
		   << ",...},mem_sz=" << phy_mem_size * sizeof(Page)
		   << ",pci=" << (void *)io_mem_phy_addr
		   << ",pci_sz=" << (void *)(io_mem_size  * sizeof(Page))
		   << ")\n";

    // Calculate the number of page tables needed to map the physical memory
    n_pts = (phy_mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;

    // Map all physical memory into the page tables pointed by phy_mem_pts
    // These will be attached at both MM::PHY_MEM and MM::APP_LO thus flags
    // must consider application access
    pts = reinterpret_cast<PT_Entry *>(pmm->phy_mem_pts);
    for(unsigned int i = 0; i < phy_mem_size; i++)
	pts[i] = (i * sizeof(Page)) | Flags::APP;

    // Setup the System Page Directory
    sys_pd = reinterpret_cast<PT_Entry *>(pmm->sys_pd);

    // Attach all physical memory starting at MM::PHY_MEM
    for(int i = 0; i < n_pts; i++)
	sys_pd[MMU::directory(MM::PHY_MEM) + i] =
	    (pmm->phy_mem_pts + i * sizeof(Page)) | Flags::SYS;

    // Attach application memory starting at "pmm->app_lo"
    for(unsigned int i = MMU::directory(
	    MMU::align_directory(pmm->app_lo));
	i < MMU::directory(MMU::align_directory(pmm->app_hi));
	i++)
	sys_pd[i] = (pmm->phy_mem_pts + i * sizeof(Page)) |
	    Flags::APP;

    // Calculate the number of page tables needed to map the PCI AS
    n_pts = (io_mem_size + MMU::PT_ENTRIES - 1) / MMU::PT_ENTRIES;

    // Map PCI addres space into the page tables pointed by io_mem_pts
    pts = reinterpret_cast<PT_Entry *>(pmm->io_mem_pts);
    for(unsigned int i = 0; i < io_mem_size; i++)
	pts[i] = (io_mem_phy_addr + i * sizeof(Page)) | Flags::PCI;

    // Attach PCI devices' memory to MM::IO_MEM
    for(int i = 0; i < n_pts; i++)
	sys_pd[MMU::directory(MM::IO_MEM) + i] =
	    (pmm->io_mem_pts + i * sizeof(Page)) | Flags::PCI;

    // Map the system 4M logical address space at the top of the 4Gbytes
    sys_pd[MMU::directory(MM::SYS_CODE)] = pmm->sys_pt | Flags::SYS;

    db<Setup>(INF) << "SPD=" << *((Page_Directory *)sys_pd) << "\n";
}

//========================================================================
// setup_lmm
//
// Desc: Setup the Logical_Memory_ap in System_Info.
//
// Parm: lmm	-> logical memory map
//	 app_hi	-> highest logicall RAM address available to applications
//------------------------------------------------------------------------
void setup_lmm(LMM * lmm, Log_Addr app_entry, Log_Addr app_hi)
{
//     lmm->int_vec = MM::INT_VEC;
//     lmm->sys_pt = MM::SYS_PT;
//     lmm->sys_pd = MM::SYS_PD;
//     lmm->sys_info = MM::SYS_INFO;
//     lmm->phy_mem = MM::PHY_MEM;
//     lmm->io_mem = MM::IO_MEM;
//     lmm->sys_code = MM::SYS_CODE;
//     lmm->sys_data = MM::SYS_DATA;
//     lmm->sys_stack = MM::SYS_STACK;
//     lmm->app_lo = MM::APP_LO;
    lmm->app_entry = app_entry;
//     lmm->app_code = MM::APP_CODE;
//     lmm->app_data = MM::APP_DATA;
    lmm->app_hi = app_hi;
//     lmm->mach1 = MM::MACH1;
//     lmm->mach2 = MM::MACH2;
//     lmm->mach3 = MM::MACH2;

    db<Setup>(INF) << "lmm={"
// 		   << "int=" << (void *)lmm->int_vec
// 		   << ",gdt="  << (void *)lmm->mach1
// 		   << ",pt="   << (void *)lmm->sys_pt
// 		   << ",pd="   << (void *)lmm->sys_pd
// 		   << ",info=" << (void *)lmm->sys_info
// 		   << ",mem="  << (void *)lmm->phy_mem
// 		   << ",io="   << (void *)lmm->io_mem
// 		   << ",cod="  << (void *)lmm->sys_code
// 		   << ",dat="  << (void *)lmm->sys_data
// 		   << ",stk="  << (void *)lmm->sys_stack
// 		   << ",apl="  << (void *)lmm->app_lo
		   << "ape="  << (void *)lmm->app_entry
// 		   << ",apc="  << (void *)lmm->app_code
// 		   << ",apd="  << (void *)lmm->app_data
		   << ",aph="  << (void *)lmm->app_hi
		   << "})\n";
}

//========================================================================
// copy_sys_info
//
// Desc: Copy the system information block, which includes boot info
//	 and the physical memory map, into its definitive place.
//
// Parm: from -> current location
// 	 to   -> destination
//------------------------------------------------------------------------
void copy_sys_info(System_Info * from, System_Info * to)
{
    db<Setup>(TRC) << "copy_sys_info(from=" << (void *)from
		   << ",to=" << (void *)to
		   << ",size=" << sizeof(System_Info) << ")\n";

    if(sizeof(System_Info) > sizeof(Page))
	db<Setup>(WRN) << "System_Info is bigger than a page ("
		       << sizeof(System_Info) << ")!\n";

    memcpy((void *)to, (void *)from, sizeof(System_Info));
}

//========================================================================
// call_next
//
// Desc: Setup a stack for the next boot stage and call it.
//
// Parm: entry -> call target
//------------------------------------------------------------------------
void call_next(Log_Addr entry)
{
    db<Setup>(TRC) << "call_next(ip=" << (void *)entry
		   << ",sp=" << (void *)(MM::SYS_STACK + TR::SYSTEM_STACK_SIZE
					 - 2 * sizeof(int))
		   << ")\n";

    db<Setup>(INF) << "PC_Setup ends here!\n\n";

    // The following is perfectly correct, but may cause VMware to crash
    // It must be used on real versions
    // The 2 integers on the stack are ???
    ASM("movl	%0, %%esp	\n"
 	"call	*%%ebx		\n"
 	: : "i"(MM::SYS_STACK + TR::SYSTEM_STACK_SIZE - 2 * sizeof(int)),
	    "b"(static_cast<unsigned int>(entry)));
}

//========================================================================
// page_fault
//
// Desc: Page fault exception handler (what a handling! :-)).
//
// Parm: exception stack and error code pushed by the CPU
//------------------------------------------------------------------------
void page_fault(Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 esp3, Reg32 ss,
		Reg32 error)
{
    db<Setup>(ERR) << "this thread generated an invalid address and will be terminated!\n";
    db<Setup>(ERR) << "address=" << (void *)CPU::cr2() << "\n";
    db<Setup>(ERR) << "context={cs=" << cs
		   << ",ip=" << (void *)eip
		   << ",flg=" << (void *)eflags
		   << ",ss=" << (void *)ss
		   << ",sp=" << (void *)esp3
		   << ",err=" << (void *)error << "}\n";

    panic();
}

//========================================================================
// gpf
//
// Desc: GPF exception handler (what a handling! :-)).   
//
// Parm: exception stack and error code pushed by the CPU
//------------------------------------------------------------------------
void gpf(Reg32 eip, Reg32 cs, Reg32 eflags, Reg32 esp3, Reg32 ss, Reg32 error)
{
    short ds;
    ASM("movw %%ds, %0" : "=o" (ds) : );

    db<Setup>(ERR) << "this thread caused a GPF and will be terminated!\n";
    db<Setup>(ERR) << "context={cs=" << cs
		   << ",ds=" << ds
		   << ",ip=" << (void *)eip
		   << ",flg=" << (void *)eflags
		   << ",ss=" << (void *)ss
		   << ",sp=" << (void *)esp3
		   << ",err=" << (void *)error << "}\n",

    panic();
}

//========================================================================
// fpu
//
// Desc: FPU exception handler.				   
//------------------------------------------------------------------------
void fpu()
{
    db<Setup>(ERR) << "FPU generated an interrupt!\n";

//    ASM("clts");

    panic();
}

//========================================================================
// syscall
//
// Desc: Linux lost syscall handler.
//------------------------------------------------------------------------
void syscall()
{
    db<Setup>(ERR) << "System call invoked but no OS kernel loaded (yet)!\n";

    panic();
}

//========================================================================
// hard_int
//
// Desc: Hardware triggered interrupt catcher.

//------------------------------------------------------------------------
void hard_int()
{
    db<Setup>(ERR)
	<< "Hardware interrupt occurred, but no handler was installed!\n";

    panic();
}

//========================================================================
// panic
//
// Desc: This function is called if something goes wrong during setup,
//	 including uncaught interrupts.
//------------------------------------------------------------------------
void panic()
{
    unsigned short * video;

    //	 Set a pointer to GCA text frame buffer so we can say something
    video = reinterpret_cast<unsigned short *>(
	Traits<PC_Display>::FRAME_BUFFER_ADDRESS);

    *video++ = 'S' | 0x1f00;
    *video++ = 'E' | 0x1f00;
    *video++ = 'T' | 0x1f00;
    *video++ = 'U' | 0x1f00;
    *video++ = 'P' | 0x1f00;
    *video++ = ':' | 0x1f00;
    *video++ = ':' | 0x1f00;
    *video++ = 'p' | 0x1f00;
    *video++ = 'a' | 0x1f00;
    *video++ = 'n' | 0x1f00;
    *video++ = 'i' | 0x1f00;
    *video++ = 'c' | 0x1f00;
    *video++ = '(' | 0x1f00;
    *video++ = ')' | 0x1f00;

    CPU::halt();
}

// This might be very useful for debugging!
//     register int sp;
//     ASM("movl %%esp, %0" : "=r"(sp) : );
//     unsigned short * video = (unsigned short *) CGA_FBUF_PHY_ADDR;
//     video [10] = ((sp >> 12) & 0xf)  | 0x1f30;
//     video [11] = ((sp >> 8)  & 0xf)  | 0x1f30;
//     video [12] = ((sp >> 4)  & 0xf)  | 0x1f30;
//     video [13] =  (sp        & 0xf)  | 0x1f30;
//     video [14] = 'O' | 0x1f00;
//     video [15] = 'K' | 0x1f00;
//     for(;;);
