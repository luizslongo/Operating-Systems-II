#ifdef __ELF__
#define TYPE(x) .type x,function
#else
#define TYPE(x)
#endif

.section .init
.align 2

        // INTERRUPT VECTORS
        ldr pc, =_start
        ldr pc, =_undefined_instruction
        ldr pc, =_software_interrupt
        ldr pc, =_prefetch_abort
        ldr pc, =_data_abort
        ldr pc, =_reserved
        ldr pc, =_irq_handler
        ldr pc, =_fiq_handler

        // These vectors and code are used for rom patching.
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


.global _init
//TYPE(_init)
.type _init,function
_init:
	.equ STACK_ADDRESS,		0x00418000 /* should be a symbol */
	.equ ARM_MODE_USR,		0x10
	.equ ARM_MODE_FIQ,		0x11
	.equ ARM_MODE_IRQ,		0x12
	.equ ARM_MODE_SVC,		0x13

	.equ IRQ_BIT, 			0x80;
	.equ FIQ_BIT,			0x40;

	msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT
	ldr sp, =STACK_ADDRESS

/* this sould not be here */

.ifdef mc13224v
    /* initialize rom data */
    .set _rom_data_init, 0x108d0
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


/* Creating some symbols...
 * These will help us constructing
 * the global objects. Is there
 * a better way?
 */

.section .ctors
.global _ctors_begin
_ctors_begin:
.section .dtors
.global _ctors_end
_ctors_end:
.global _dtors_begin
_dtors_begin:
