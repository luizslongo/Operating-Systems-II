// EPOS-- ML310 Boot Strap

#include <utility/elf.h>
#include <machine.h>

__USING_SYS

// System_Info Imports
typedef System_Info<ML310> SI;
typedef Memory_Map<ML310> MM;
typedef void (function_type)();

// Constants
static const unsigned int BOOT_IMAGE = 9 * 1024;

// Prototypes
extern "C" { void _start(); }

void _start()
{
    //Disable Interrupts
    //Already disabled at startup ? - chack later

    // GPIO Doc ...
    //  0b0000 0000 0000 0000 0000 0000 0000 0000
    //                                  |||| ||||-DBG7
    //                                  |||| |||--DBG6
    //                                  |||| ||---DBG5
    //                                  ..............
    //                                  |---------DBG0
    //Light on GPIO Leds

    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR+4)) = 0x00000000;
    //Light Stage1 Boot.
    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR)) = 0xFFFFFFFE;

    // Setting a stack for EPOS initialization
    unsigned int stack_pointer = MM::SYS_STACK; 
    ASMV("mr 1,%0" : : "r" (stack_pointer));

    db<Boot>(TRC) << "BOOT()\n";

    // The boot image was loaded by the monitor at BOOT_IMAGE
    char * tmp = reinterpret_cast<char *>(BOOT_IMAGE);

    // System_Info is the first thing in the boot image
    SI * si = reinterpret_cast<SI *>(tmp);

    // And will be moved to the place indicated in Traits
    char * bi = reinterpret_cast<char *>(Traits<ML310>::BOOT_IMAGE_ADDR);
    memcpy(bi, tmp, si->bm.img_size);

    // Reset si to its definitive location
    si = reinterpret_cast<SI *>(bi);

    // Load SETUP
    ELF * stp_elf = reinterpret_cast<ELF *>(&bi[si->bm.setup_offset]);
    if(!stp_elf->valid()) {
	db<Boot>(ERR) << "SETUP ELF image was corrupted!\n";
	Machine::panic();
    }
    if(stp_elf->load_segment(0) < 0) {
	db<Boot>(ERR) << "SETUP code segment was corrupted!\n";
	Machine::panic();
    }
    for(int i = 1; i < stp_elf->segments(); i++) {
	if(stp_elf->load_segment(i) < 0) {
	    db<Boot>(ERR) << "SETUP data segment was corrupted!\n";
	    Machine::panic();
	}
    }

    // Call setup
    void (*entry)() = reinterpret_cast<void (*)()>(stp_elf->entry());

    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR)) = 0xFFFFFFFC;

    entry();
}
