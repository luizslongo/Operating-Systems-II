#include "boot.S"
.file "armv7_crti.S"

.section .init
.align 2

        /* INTERRUPT VECTORS */
_vector_table:
        ldr pc, _start_addr
        ldr pc, _undefined_instruction_addr
        ldr pc, _software_interrupt_addr
        ldr pc, _prefetch_abort_addr
        ldr pc, _data_abort_addr
        ldr pc, _reserved_addr
        ldr pc, _irq_handler_addr
        ldr pc, _fiq_handler_addr

        /* These vectors and code are used for rom patching.*/
        .org 0x20
        .code 16
        _RPTV_0_START:
        bx lr

        .org 0x60
        _RPTV_1_START:
        bx lr

        .org 0xa0
        _RPTV_2_START:
        bx lr

        .org 0xe0
        _RPTV_3_START:
        bx lr

        .org 0x120
        ROM_var_start: .word 0
        .org 0x7ff
        ROM_var_end:   .word 0
        .code 32

_start_addr:                .word _init /*_start*/
_undefined_instruction_addr:.word _undefined_instruction
_software_interrupt_addr:   .word _prefetch_abort
_prefetch_abort_addr:       .word _undefined_instruction
_data_abort_addr:           .word _data_abort
_reserved_addr:             .word _reserved
_irq_handler_addr:          .word _irq_handler
_fiq_handler_addr:          .word _fiq_handler

/*_stack_addr: .word 0x00418000 */ /* should be a symbol */
_stack_addr: .word 0x1fffffff /* arbitrary from memory_map.h (TOP) */

.equ ARM_MODE_USR,		0x10
.equ ARM_MODE_FIQ,		0x11
.equ ARM_MODE_IRQ,		0x12
.equ ARM_MODE_SVC,		0x13

.equ IRQ_BIT, 			0x80;
.equ FIQ_BIT,			0x40;

# The values below are fairly arbitrary
#0x00100000 = 1mb
#.set _vector_table,		vector_table
.set MMUTable,			0x00104000 
.set __stack,			0x0010c060 /*32864 bytes, sys stack*/
.set __irq_stack,		0x0010e060 /*8192 bytes*/
.set __supervisor_stack,0x0010e860 /*2048 bytes*/
.set __abort_stack,		0x0010ec60 /*1024 bytes*/
.set __fiq_stack,		0x0010f060 /*1024 bytes*/
.set __undef_stack,		0x0010f460 /*1024 bytes*/


.global _init
.type _init,function
_init:

	add r0, r0, #4
	add r0, r0, #5
	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT
	and r0, r0, r0
	and r0, r0, r0
	ldr sp, _stack_addr
	and r0, r0, r0
	and r0, r0, r0
	and r0, r0, r0

	/* test if pandaboard turn leds off */
	/*movw	r3, #0
	movt	r3, #18993 
	ldr	r2, [r3, #308]
	bic	r2, r2, #448
	str	r2, [r3, #308]
	testloop:
	b	testloop*/
	/*
	// Testing a simple write
	ldr r0, =0xe0001020
	ldr r1, =0xD
	str r1, [r0]
	ldr r0, [r0]
	*/

	/* boot code */

	/*b _boot

	.global boot_return
	boot_return:
*/
	/* boot code end */



/* clear bss */
    mov r0, #0
    ldr r1, =__bss_start__
    ldr r2, =__bss_end__
  
    bss_clean_loop:
        cmp r1, r2
        strlo r0, [r1], #4
        blo bss_clean_loop

    bl __do_global_ctors_aux

.section .fini
.global _fini
_fini:
    b _fini
