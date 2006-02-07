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
#include <machine.h>

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
    if((elf->segments() - 1) != 1)
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

