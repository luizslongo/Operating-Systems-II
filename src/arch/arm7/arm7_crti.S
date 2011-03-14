
	.section .init

/* ---- INTERRUPT VECTORS ---- */

	ldr pc, reset_addr 
	ldr pc, undef_hand_addr
	ldr pc, undef_hand_addr
	ldr pc, undef_hand_addr
	ldr pc, data_abort_addr
	nop
	ldr pc, irq_hand_addr
	ldr pc, undef_hand_addr

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

	ldr r12, =_rom_data_init
	mov lr, pc
	mov pc, r12

	/* Back to our archteture initialization */
	
reset_addr: .word reset
undef_hand_addr: .word undef_hand
data_abort_addr: .word data_abort
irq_hand_addr: .word irq_hand

/* ---- General interruption handlers ---- */

undef_hand:
	b undef_hand

data_abort:
	b data_abort

/* ---- Reset interruption handler ---- */

reset:
	.global _start
	b _start

_start:
	.equ STACK_ADDRESS,		0x00418000 /* should be a symbol */
	.equ IRQ_STACK_SIZE,	(3*8*4)
	.equ ARM_MODE_USR,		0x10
	.equ ARM_MODE_IFQ,		0x11
	.equ ARM_MODE_IRQ,		0x12
	.equ ARM_MODE_SVC,		0x13

	.equ IRQ_BIT, 			0x80;
	.equ IFQ_BIT,			0x40;

	.set _rom_data_init, 0x108d0

	/* Defining special stacks... */

	ldr r0, =STACK_ADDRESS
	msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | IFQ_BIT
	mov sp, r0

	/*sub r0, r0, #IRQ_STACK_SIZE
	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | IFQ_BIT
	mov sp, r0*/

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

	/* Initialize ROM variables */
	ldr r12, =_rom_data_init
	mov lr, pc
	mov pc, r12

	/* Calling main! */
	ldr lr, =exit
	ldr r0, =main
	bx r0

exit:
	b exit

	/* TODO: destruct and halt! */

/* ---- IRQ Handler ---- */

irq_hand:	

	mov r13, r0
	sub lr, lr, #4
	mov r0, lr

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | IFQ_BIT

	stmfd sp!, {r0}
	stmfd sp!, {r1-r12, lr}

	mov r0, sp

	msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | IFQ_BIT

	mrs r14, spsr
	stmfd r0!, {r13, r14}

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | IFQ_BIT

	sub sp, sp, #8

	ldr r0, =__epos_irq_handler
	mov lr, pc
	bx  r0

	ldmfd sp!, {r0, r1}
	msr spsr_cfxs, r1
	ldmfd sp!, {r1-r12, lr}
	ldmfd sp!, {pc}^
	

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



