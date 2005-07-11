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

    static const int SYSCALL_INT = Traits::SYSCALL_INT;
    
    typedef IA32::Reg32 Reg32;
    typedef IA32::ISR ISR;
    typedef IA32::FSR FSR;

public:
    // Interrupts
    enum {
   	INT_BASE        = Traits::INT_BASE,
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
    ASM("pushal");
    ASM("call *%0" : : "r"(r));
    // Send EOI to the PIC (what about the cascaded PIC?
    ASM("movb	$0x20, %al\n" 
  	"outb	%al, $0x20\n"); 
    ASM("popal");
    ASM("iret");
}

template <PC::FSR * r>
void PC::fsr_wrapper() {
    ASM("call *%0" : : "r"(r));
    ASM("iret");
}

typedef PC Machine;

__END_SYS

#endif
