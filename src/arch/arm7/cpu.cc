// EPOS-- ARM7 CPU Mediator Implementation

#include <arch/arm7/cpu.h>

__BEGIN_SYS

void ARM7::Context::save() volatile
{

}

void ARM7::Context::load() const volatile
{
	db<CPU>(TRC) << "CPU::Context::load(this=" << (void*)this << ")\n";

	ASMV("ldmfd %0, {r0-r12,lr,pc,sp}\n"
		 :
		 : "r" (this)
		 :);
}


/* This method is provided as "naked" in cpu.h.
 * The compiler doesn't generate prologue neither
 * epilogue for this call
 */
void ARM7::switch_context(Context * volatile * o, Context * volatile n)
{
	/* 'o' and 'n' were saved by default respectively in
	 * sp+4 and sp+0, but they still also in r0 and r1
	 */

	ASMV("ldr r0, [r0, #0]\n"
		 "stmia r0, {r0-r12,lr,sp}\n"
		 "str lr, [r0, #60]\n" // overwriting pc with lr
		 "mrs r3, cpsr\n"
		 "str r3, [r0, #64]\n"
		 "ldr r3, [sp, #4]\n"
		 "str r3, [r0, #0]\n"
		 "ldr r3, [r1, #64]\n"
		 "msr spsr_cfsx, r3\n"
		 "ldmfd r1, {r0-r12,lr,pc,sp}^\n"
		);
}


__END_SYS

