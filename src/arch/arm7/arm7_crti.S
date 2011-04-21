
	.section .init

/* ---- INTERRUPT VECTORS ---- */

	ldr pc, reset_addr 
	ldr pc, undef_hand_addr
	ldr pc, sw_inter_addr
	ldr pc, prefetch_abort_addr
    ldr pc, data_abort_addr
    ldr pc, reserved_addr
	ldr pc, irq_hand_addr
	ldr pc, fiq_hand_addr

	/* These vectors and code are used for rom patching.
	 * Someday it should be at mach/mc13224v directory
	 */

	.org 0x20
	.code 16
	_RPTV_0_START:
	bx lr /* do nothing */

	.org 0x60
	_RPTV_1_START:
	bx lr /* do nothing */

	.org 0xa0
	_RPTV_2_START:
	bx lr /* do nothing */

	.org 0xe0
	_RPTV_3_START:
	bx lr /* do nothing */

	.org 0x120
	ROM_var_start: .word 0
	.org 0x7ff
	ROM_var_end:   .word 0
	.code 32

	/* Back to our archteture initialization */
	
reset_addr: .word _reset
undef_hand_addr: .word _undefined_instruction
sw_inter_addr: .word _software_interrupt
prefetch_abort_addr: .word _prefetch_abort
data_abort_addr: .word _data_abort
reserved_addr: .word _reserved
irq_hand_addr: .word irq_hand
fiq_hand_addr: .word _fiq_handler

//TODO: move exception handling elsewhere (setup?)

/* ---- Reset interruption handler ---- */

_reset:
	.global _start
	b _start

_start:
	.equ STACK_ADDRESS,		0x00418000 /* should be a symbol */
	.equ ARM_MODE_USR,		0x10
	.equ ARM_MODE_FIQ,		0x11
	.equ ARM_MODE_IRQ,		0x12
	.equ ARM_MODE_SVC,		0x13

	.equ IRQ_BIT, 			0x80;
	.equ FIQ_BIT,			0x40;

	.set _rom_data_init, 0x108d0

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT
	ldr sp, =STACK_ADDRESS

/* this sould not be here */

.ifdef mc13224v
    /* initialize rom data */
    ldr r12, =_rom_data_init
    mov lr, pc
    mov pc, r12
.endif

	/* Cleaning the bss section */

	mov r0, #0
	ldr r1, =__bss_start__
	ldr r2, =__bss_end__

	bss_clean_loop:
		cmp r1, r2
		strlo r0, [r1], #4
		blo bss_clean_loop

	/* Constructing global objects */

	ldr r0, =_ctors_end - 4
	ldr r1, =_ctors_begin - 4

	ctors_loop:
		cmp r0, r1
		beq ctors_loop_end
		ldr r2, [r0], #-4
		stmfd sp!, {r0-r1}
		mov lr, pc
		bx r2
		ldmfd sp!, {r0-r1}
		b ctors_loop

	ctors_loop_end:

    /* This piece of code should not be reachable
     * when Thread::enabled = true. In this case,
     * the main thread context should be loaded
     * in the last constructor.
     */
	ldr lr, =exit
	ldr pc, =main

exit:
	b exit

	/* TODO: destruct and halt! */

/* ---- IRQ Handler ---- */

irq_hand:	

	mov r13, r0    // backup contents of r0 in an unused register
	sub lr, lr, #4 // adjust value to return to
	mov r0, lr     // move return address to r0 so I can pass it to SVC mode

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT // go to SVC

	stmfd sp!, {r0}         // push address to return from interrupt
	stmfd sp!, {r1-r12, lr} // save current context

	mov r0, sp  // move SVC stack pointer to r0 so I can pass it to IRQ mode

	msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | FIQ_BIT // go to IRQ

	mrs r14, spsr         // backup spsr to a unused register
	stmfd r0!, {r13, r14} // save both return address and spsr to SVC stack

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT // go back to SVC

	mov sp, r0 // update stack pointer

    bl _irq_handler // call system handler

	ldmfd sp!, {r0, r1}     // restore original value of r0 and value of spsr
	msr spsr_cfxs, r1       // restore IRQ's spsr value to SVC's spsr
	ldmfd sp!, {r1-r12, lr} // restore original execution context
	ldmfd sp!, {pc}^        // return from interrupt ('^' forces copy of spsr to cpsr)

/* Creating some symbols...
 * These will help us constructing
 * the global objects. Is there 
 * a better way?
 */

.section .ctors
_ctors_begin:
.section .dtors
_ctors_end:
_dtors_begin:



