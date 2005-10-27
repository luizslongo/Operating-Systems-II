// EPOS-- PC Mediator Declarations

#ifndef __pc_h
#define __pc_h

#include <machine.h>
#include <arch/ia32/cpu.h>
#include <arch/ia32/mmu.h>
#include <arch/ia32/tsc.h>
#include "memory_map.h"
#include "pci.h"
#include "ic.h"
#include "timer.h"
#include "rtc.h"
#include "display.h"
#include "uart.h"
#include "sensor.h"

__BEGIN_SYS

class PC: public Machine_Common
{
private:
    typedef Traits<PC> Traits;
    static const Type_Id TYPE = Type<PC>::TYPE;

    static const unsigned int HARD_INT = Traits::HARD_INT;
    static const unsigned int SYSCALL_INT = Traits::SYSCALL_INT;
    
    typedef IA32::Reg32 Reg32;
    typedef IA32::ISR ISR;
    typedef IA32::FSR FSR;

public:
    // Hardware Interrupts
    enum {
	INT_BASE        = HARD_INT,
        INT_TIMER       = INT_BASE + 0,
        INT_KEYBOARD    = INT_BASE + 1
    };        

public:
    PC() {}
  
    static ISR * int_handler(int i) {
	IA32::IDT_Entry * idt = 
	    reinterpret_cast<IA32::IDT_Entry *>(Memory_Map<PC>::INT_VEC);
	if(i < IA32::IDT_ENTRIES)
	    return reinterpret_cast<ISR *>(idt[i].offset());
    }
    static void int_handler(int i, ISR * r) {
	IA32::IDT_Entry * idt = 
	    reinterpret_cast<IA32::IDT_Entry *>(Memory_Map<PC>::INT_VEC);
	if(i < IA32::IDT_ENTRIES)
	    idt[i] = IA32::IDT_Entry(IA32::GDT_SYS_CODE,
				     reinterpret_cast<Reg32>(r),
				     IA32::SEG_IDT_ENTRY);
    }

    static void panic();
    static void int_not();
    static void exc_not(Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_pf(Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_gpf(Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);
    static void exc_fpu(Reg32 error, Reg32 eip, Reg32 cs, Reg32 eflags);

    template <ISR * r> static void isr_wrapper();
    template <FSR * r> static void fsr_wrapper();


    static int init(System_Info * si);

public:
    IA32 cpu;
    IA32_MMU mmu;
    IA32_TSC tsc;
    PC_IC ic;
    PC_PCI pci;
    PC_Display display;
};

// These wrappers must be defined out of class to prevent inlining
// Units using (instantiating) them must be compiled with -fomit-frame-pointer
template <PC::ISR * r>
void PC::isr_wrapper() 
{
    // Flags are implicitly pushed by the CPU's interrupt dispatcher
    // Interrupts are kept enabled (driver can disable it if necessary)
    ASM("	pushal			# save regs	\n");
    ASM("	call	*%0		# call ISR	\n" : : "r"(r));
    ASM("	movb	$0x0b, %al	# which IRQ?	\n"
	"	outb	%al, $0x20			\n"
	"	inb	$0x20, %al	# -> IRQ	\n"
	"	andb    $4, %al		# IRQ2 (slave)?	\n"
	"	testb	%al, %al			\n"
	"	je	.L1				\n"
	"	movb	$0x20, %al	# EIO		\n" 
	"	outb	%al, $0xa0	# EOI -> slave	\n" 
	".L1:	movb	$0x20, %al	# EIO		\n" 
 	"	outb	%al, $0x20	# EOI -> master	\n");
    ASM("	popal			# restore regs	\n"
	"	iret					\n");
}

template <PC::FSR * r>
void PC::fsr_wrapper() {
    ASM("	call	*%0						\n"
	"	iret							\n"
	: : "r"(r));
}

typedef PC Machine;

__END_SYS

#endif
