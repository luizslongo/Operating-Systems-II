// EPOS-- IA32 CPU Mediator Implementation

#include <arch/ia32/cpu.h>

__BEGIN_SYS

void IA32::Context::save() volatile
{
    ASM("	pushl	%ebp						\n"
 	"	movl	%esp, %ebp					\n"
	"	movl    8(%ebp), %esp	# this		 		\n"
	"	addl    $40, %esp      	# + sizeof(Context)		\n"
	"	pushl	4(%ebp)		# eip				\n"
	"	pushfl							\n"
	"	pushl	%eax						\n"
	"	pushl	%ecx						\n"
	"	pushl	%edx						\n"
	"	pushl	%ebx						\n"
	"	pushl   %ebp		# esp				\n"
	"	pushl   (%ebp)		# ebp				\n"
	"	pushl	%esi						\n"
	"	pushl	%edi						\n"
	"	movl    %ebp, %esp					\n"
	"	popl    %ebp						\n");
}

void IA32::Context::load() const volatile
{
    // POPA saves an extra "esp" (which is always "this"),
    // but saves several instruction fetchs
    ASM("	movl    4(%esp), %esp	# this	\n"
	"	popal				\n"
 	"	popfl				\n");
}

void IA32::switch_context(Context * volatile * o, Context * volatile n)
{
    // The same here: one extra push and one extra pop, but several fecths less
    ASM("	pushfl				\n"
	"	pushal				\n"
	"	movl    40(%esp), %eax	# old	\n" 
	"	movl    %esp, (%eax)		\n"
	"	movl    44(%esp), %esp	# new	\n"
	"	popal				\n"
	"	popfl				\n");
}

__END_SYS
