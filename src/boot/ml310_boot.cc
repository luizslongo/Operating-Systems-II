// ML310_BOOT
//
// Author: Hugo
// Documentation: $EPOS/doc/boot                        Date: 04 Dec 2005

#include <arch/ppc32/cpu.h>
#include <mach/ml310/memory_map.h>
#include <utility/string.h>

__USING_SYS

// System_Info Imports
typedef Memory_Map<ML310> MM;
typedef unsigned int (function_type)(int,int);

// Prototypes
extern "C" { void _start(); }

//========================================================================
// _start
//
// Desc:
//
//------------------------------------------------------------------------
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

    // The boot image at BOOT_IMAGE_ADDR
    char * bi = (char *) (Traits<ML310>::BOOT_IMAGE_ADDR);

    // Get the System_Info, the first thing after boot
    System_Info * si = ( (System_Info *) bi);

    //Getting setup entry point and its size
    register unsigned int addr = (int)Traits<ML310>::SETUP_ADDR;
    register unsigned int size = (si->bm.loader_off - si->bm.setup_off);

    // Skips 84 bytes on bootimage for ELF Header !
    memcpy((void *)addr, (void *)&(bi[si->bm.setup_off + 84]), (size - 84));
    //memcpy((void *)addr, (void *)&(bi[si->bm.setup_off]), (size));

    // Call setup main()
    function_type * function = (function_type *) addr;

    *((volatile unsigned int *)(Traits<Machine>::LEDS_BASEADDR)) = 0xFFFFFFFC;

    function(addr,size);
}
