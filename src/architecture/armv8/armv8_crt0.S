// EPOS ARMv7 Program Starter

        .file "armv7_crt0.S"
        .extern _sys_info
        .global _sys_info
        .type _sys_info, object
_sys_info:
        //boot map info
        .long 0x0 //n_cpus
        .long 0x0 //mem_base
        .long 0x0 //mem_top
        .long 0x0 //io_base
        .long 0x0 //io_top
        .long 0x0 //node_id
        .long 0x0 //space_x
        .long 0x0 //space_y
        .long 0x0 //space_z
        .long 0x0, 0x0 // equivalent to ".byte 0x0,0x0,0x0,0x0,0x0,0x0,0x0,0x0" = uuid
        .long 0x0 //img_size
        .long 0x0 //setup_offset
        .long 0x0 //init_offset
        .long 0x0 //system_offset
        .long 0x0 //application_offset
        .long 0x0 //extras_offset
        //load map info
        .byte 0x0 //has_ext
        .long 0x0 //app_entry;
        .long 0x0 //app_segments;
        .long 0x0 //app_code;
        .long 0x0 //app_code_size;
        .long 0x0 //app_data;
        .long 0x0 //app_stack;
        .long 0x0 //app_heap;
        .long 0x0 //app_data_size;
        .long 0x0 //app_extra;
        .long 0x0 //app_extra_size;

        // Entry point called by SETUP for microcontrollers
        .section .text
        .align  2
        .global _mcu_start
        .type   _mcu_start, function
_mcu_start:
        // Temporary Stack Pointer for INIT
        // Main's stack will be allocated by Thread::init()  
        ldr     r0, =__boot_stack__
        mov     sp, r0

        // Clear the BSS
        eor     r0, r0
        ldr     r1, =__bss_start__
        ldr     r2, =__bss_end__
.L1:    str     r0, [r1]
        add     r1, #4
        cmp     r1, r2
        blt     .L1

        // Entry point called by SETUP for processors
        .section .text
        .align  2
        .global _start
        .type   _start, function
_start:
        // Temporary stack(s) for INIT are created and configure by SETUP

        // Call _init
        bl      _init

        // Call main
        .align  2
        .globl  __epos_app_entry
        .type   __epos_app_entry, function
__epos_app_entry:
        bl      __pre_main
        bl      main
        push    {r0}         // save main's return value to be used by exit()
        bl      _fini
        bl      _exit
