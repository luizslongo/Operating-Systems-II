#include <mach/integratorcp/ic.h>

__BEGIN_SYS

IntegratorCP_IC::Interrupt_Handler IntegratorCP_IC::_vector[29] =
{ 
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
    &IntegratorCP_IC::default_handler 
};

CPU::Reg32 IntegratorCP_IC::_mask = 0;

void IntegratorCP_IC::handle_int() {
    CPU::Reg32 status = CPU::in32(PIC_IRQ_STATUS);
        
    // search for interrupt id
    Interrupt_Id id = 0;
    CPU::Reg32 s = status;
    while (s >>= 1) {
        ++id;
    };
    
    db<IC>(TRC) << "IC::handle_int(id="<<id<<")\n";
   
    // call handler
    _vector[id](id);
    db<IC>(TRC) << "return from handler\n";
}

void IntegratorCP_IC::default_handler(Interrupt_Id id) {
    db<IntegratorCP_IC>(INF) << "IntegratorCP_IC::default_handler(id="<<id<<")\n";    
}

extern "C" {

void _undefined_instruction() __attribute__ ((naked));
void _undefined_instruction()
{
    kout << "undefined instruction\n";
    ASMV("movs pc, r14");
}

void _software_interrupt() __attribute__ ((naked));
void _software_interrupt()
{
    kout << "software interrupt\n";
    ASMV("movs pc, r14");
}

void _prefetch_abort() __attribute__ ((naked));
void _prefetch_abort()
{
    kout << "prefetch abort\n";
    ASMV("subs pc, r14, #4");
}

void _data_abort() __attribute__ ((naked));
void _data_abort()
{
    kout << "data abort\n";
    ASMV("subs pc, r14, #8");
}

void _reserved() __attribute__ ((naked));
void _reserved()
{
    kout << "reserved\n";
    ASMV("mov pc, r14");
}

void _irq_handler() __attribute__ ((naked));
void _irq_handler() {

    ASMV(
            // A few definitions
            ".equ ARM_MODE_USR,      0x10 \n"
            ".equ ARM_MODE_FIQ,      0x11 \n"
            ".equ ARM_MODE_IRQ,      0x12 \n"
            ".equ ARM_MODE_SVC,      0x13 \n"
            ".equ IRQ_BIT,           0x80 \n"
            ".equ FIQ_BIT,           0x40 \n"

            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT \n" // go to SVC

            "stmfd sp!, {r0-r3,r12,lr,pc}\n" // save current context

            "msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | FIQ_BIT     \n" // go to IRQ

            "sub r0, lr, #4 \n" // return from irq addr
            "mrs r1, spsr   \n" // pass irq_spsr to svn r1

            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT     \n" // go back to SVC
            "add r2, sp, #24 \n"
            "str r0, [r2, #0] \n" // save address to return from interrupt 
            "stmfd sp!, {r1} \n"   // save irq-spsr
            
            
    );            
    IC::handle_int();
    ASMV(        
            "ldmfd sp!, {r0}              \n"
            "msr spsr_cfxs, r0           \n" // restore IRQ's spsr value to SVC's spsr

            "ldmfd sp!, {r0-r3,r12,lr,pc}^ \n" // restore context
        );
}

void _fiq_handler() __attribute__ ((naked));
void _fiq_handler()
{
    kout << "fiq handler\n";
    ASMV("subs pc, r14, #4");
}

}; // extern C

__END_SYS
