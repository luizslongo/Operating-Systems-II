// EPOS-- PC SETUP Entry Point

#include <utility/elf.h>
#include <machine.h>

__USING_SYS

extern "C" { void _start(); }
extern "C" { void setup(char * bi); }

__BEGIN_SYS
extern OStream kout;
extern OStream kerr;
__END_SYS

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
    if(elf->segments() != 1)
 	Machine::panic();

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
