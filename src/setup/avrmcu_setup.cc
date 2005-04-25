#include <system/config.h>
#include <mach/avrmcu/avrmcu.h>

__USING_SYS

extern "C" unsigned __bss_end;

AVRMCU::Handler __iv[Traits<AVRMCU>::INT_VEC_SIZE];
AVRMCU::Handler * AVRMCU::interrupt_vector = __iv;

extern "C" void __setup() {

    System_Info * __si;

    __si = reinterpret_cast<System_Info *>(Memory_Map<Machine>::SYS_INFO);

    __si->bm.setup_off	= -1;
    __si->bm.system_off	= -1;

    __si->pmm.sys_info	= (unsigned)&__si;

    __si->pmm.free	= (unsigned)&__bss_end;
    __si->pmm.free_size	= Memory_Map<Machine>::SYS_INFO - (unsigned)&__bss_end;

    __si->lmm.int_vec	= (unsigned)__iv;

    return;
    
}

extern "C" void __vector_handler(void) 
                __attribute__ ((signal)) __attribute__ ((naked));

extern "C" void __vector_handler(void) {
    register unsigned char offset asm("r17");
    ASMV(
        // Save context
        // Zero (R1), Temp (R0) and SREG(0x3f)
        "push        r1"                 "\n"
        "push        r0"                 "\n"
        "in          r0, 0x3f"           "\n"
        "push        r0"                 "\n"
        // Call-saved registers
        "push        r17"                "\n"
        "push        r28"                "\n"
        "push        r29"                "\n"
        // Get offset
        "in          r29, 0x3e"          "\n"
        "in          r28, 0x3d"          "\n"
        "ldd         %0, Y+8"            "\n"
        // Fill empty stack spaces
        "ldd         r1, Y+2"            "\n"
        "std         Y+8, r1"            "\n"
        "ldd         r1, Y+1"            "\n"
        "std         Y+7, r1"            "\n"
        "adiw        r28, 2"             "\n"
        "out         0x3e, r29"          "\n"
        "out         0x3d, r28"          "\n"
        // Zero = 0
        "clr         r1"                 "\n"
        // Call-used registers
        "push        r18"                "\n"
        "push        r19"                "\n"
        "push        r20"                "\n"
        "push        r21"                "\n"
        "push        r22"                "\n"
        "push        r23"                "\n"
        "push        r24"                "\n"
        "push        r25"                "\n"
        "push        r26"                "\n"
        "push        r27"                "\n"
        "push        r30"                "\n"
        "push        r31"                "\n"
        : "=r" (offset)
        :
    );

    offset = (offset >> 1) - 1;
     
    if(__iv[offset]) 
        __iv[offset]();

    ASMV(
        "pop         r31"                "\n"
        "pop         r30"                "\n"
        "pop         r27"                "\n"
        "pop         r26"                "\n"
        "pop         r25"                "\n"
        "pop         r24"                "\n"
        "pop         r23"                "\n"
        "pop         r22"                "\n"
        "pop         r21"                "\n"
        "pop         r20"                "\n"
        "pop         r19"                "\n"
        "pop         r18"                "\n"
        // Call-saved registers
        "pop         r17"                "\n"
        // Zero (R1), Temp (R0) and SREG(0x3f)
        "pop         r0"                 "\n"
        "out         0x3f, r0"           "\n"
        "pop         r0"                 "\n"
        "pop         r1"                 "\n"
         // Rest of call-saved registers
        "pop         r29"                "\n"
        "pop         r28"                "\n"
        // Return
        "reti"                           "\n"
    );

}








