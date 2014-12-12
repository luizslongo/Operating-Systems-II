// EPOS IA32 CPU Mediator Implementation

#include <arch/ia32/cpu.h>

__BEGIN_SYS

// Class attributes
unsigned int IA32::_cpu_clock;
unsigned int IA32::_bus_clock;

// Class methods
void IA32::Context::save() volatile
{
    ASM("	pushl	%ebp						\n"
 	"	movl	%esp, %ebp					\n"
        "	movl    8(%ebp), %esp	# sp = this	 		\n"
        "	addl    $40, %esp      	# sp += sizeof(Context)		\n"
        "	pushl	4(%ebp)		# push eip			\n"
        "	pushfl							\n"
        "	pushl	%eax						\n"
        "	pushl	%ecx						\n"
        "	pushl	%edx						\n"
        "	pushl	%ebx						\n"
        "	pushl   %ebp		# push esp			\n"
        "	pushl   (%ebp)		# push ebp			\n"
        "	pushl	%esi						\n"
        "	pushl	%edi						\n"
        "	movl    %ebp, %esp					\n"
        "	popl    %ebp						\n");
}

void IA32::Context::load() const volatile
{
    // Pop the context pushed into the stack during thread creation to initialize the CPU's context, discarding user-level stack pointer
    // Obs: POPA ignores the ESP saved by PUSHA. ESP is just normally incremented
    ASM("	mov     4(%esp), %esp	# sp = this			\n"
        "	popa							\n"
 	"	popf							\n");
}

void IA32::switch_context(Context * volatile * o, Context * volatile n)
{
    // Save the previously running thread context ("o") into its stack (including the user-level stack pointer stored in the dummy TSS)
    // and updates the its _context attribute
    // PUSHA saves an extra "esp" (which is always "this"), but saves several instruction fetches
    ASM("	pushf				\n"
        "	pusha				\n"
        "	mov     40(%esp), %eax	# old	\n" 
        "	mov     %esp, (%eax)		\n");
        
    // Restore the next thread context ("n") from its stack (and the user-level stack pointer, updating the dummy TSS)
        
    ASM("	movl    44(%esp), %esp	# new	\n"
        "	popal				\n"
        "	popfl				\n");
}

__END_SYS
