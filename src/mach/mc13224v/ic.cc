// EPOS-- MC13224V IC Mediator Implementation

#include <mach/mc13224v/ic.h>

#include <machine.h>
#include <system/memory_map.h>

__BEGIN_SYS

MC13224V_IC::Interrupt_Handler MC13224V_IC::vector[11];

__END_SYS

__USING_SYS

extern "C" {

//ASMV(".global _undefined_instruction");
void _undefined_instruction() __attribute__ ((naked));
void _undefined_instruction()
{
    kout << "undefined instruction\n";
    ASMV("movs pc, r14");
}

//ASMV(".global _software_interrupt");
void _software_interrupt() __attribute__ ((naked));
void _software_interrupt()
{
    kout << "software interrupt\n";
    ASMV("movs pc, r14");
}

//ASMV(".global _prefetch_abort");
void _prefetch_abort() __attribute__ ((naked));
void _prefetch_abort()
{
    kout << "prefetch abort\n";
    ASMV("subs pc, r14, #4");
}

//ASMV(".global _data_abort");
void _data_abort() __attribute__ ((naked));
void _data_abort()
{
    kout << "data abort\n";
    ASMV("subs pc, r14, #8");
}

//ASMV(".global _reserved");
void _reserved() __attribute__ ((naked));
void _reserved()
{
    kout << "reserved\n";
    ASMV("mov pc, r14");
}

//ASMV(".global _irq_handler");
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

            "mov r13, r0    \n" // backup contents of r0 in an unused register
            "sub lr, lr, #4 \n" // adjust value to return to
            "mov r0, lr     \n" // move return address to r0 so I can pass it to SVC mode

            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT \n" // go to SVC

            "stmfd sp!, {r0}             \n" // push address to return from interrupt
            "stmfd sp!, {r1-r12, lr}     \n" // save current context

            "mov r0, sp      \n" // move SVC stack pointer to r0 so I can pass it to IRQ mode

            "msr cpsr_c, #ARM_MODE_IRQ | IRQ_BIT | FIQ_BIT     \n" // go to IRQ

            "mrs r14, spsr             \n" // backup spsr to a unused register
            "stmfd r0!, {r13, r14}     \n" // save both return address and spsr to SVC stack

            "msr cpsr_c, #ARM_MODE_SVC | IRQ_BIT | FIQ_BIT     \n" // go back to SVC

            "mov sp, r0     \n" // update stack pointer
        );

            // call system handler
            MC13224V_IC::int_handler(CPU::in16(IO_Map<Machine>::ITC_NIPEND));

    ASMV(
            "ldmfd sp!, {r0, r1}         \n" // restore original value of r0 and value of spsr
            "msr spsr_cfxs, r1           \n" // restore IRQ's spsr value to SVC's spsr
            "ldmfd sp!, {r1-r12, lr}     \n" // restore original execution context
            "ldmfd sp!, {pc}^            \n" // return from interrupt ('^' forces copy of spsr to cpsr)
        );
}

//ASMV(".global _fiq_handler");
void _fiq_handler() __attribute__ ((naked));
void _fiq_handler()
{
    kout << "fiq handler\n";
    ASMV("subs pc, r14, #4");
}

};

void MC13224V_IC::int_handler(CPU::Reg16 pending) {

    db<MC13224V_IC> (TRC) << "MC13224V_IC::int_handler(" << (int)pending << ")\n";

    switch (pending) {

    case 1 << IRQ_TIMER:
        db<MC13224V_IC> (INF) << "Interrupt! IRQ_TIMER\n";
        vector[IRQ_TIMER]();
        break;

    case 1 << IRQ_UART1:
        db<MC13224V_IC> (INF) << "Interrupt! IRQ_UART1\n";
        vector[IRQ_UART1]();
        break;

    case 1 << IRQ_UART2:
        db<MC13224V_IC> (INF) << "Interrupt! IRQ_UART2\n";
        vector[IRQ_UART2]();
        break;

    case 1 << IRQ_MACA:
        db<MC13224V_IC> (INF) << "Interrupt! IRQ_MACA\n";
        vector[IRQ_MACA]();
        break;

    default:
        db<MC13224V_IC> (INF) << "Unexpected Interrupt! => " << (unsigned int)pending << "\n";
        Machine::panic();
        break;
    }
}

//ASMV(
//        // INTERRUPT VECTORS
//        ".section .init                     \n"
//        "ldr pc, =_start                    \n"
//        "ldr pc, =_undefined_instruction    \n"
//        "ldr pc, =_software_interrupt       \n"
//        "ldr pc, =_prefetch_abort           \n"
//        "ldr pc, =_data_abort               \n"
//        "ldr pc, =_reserved                 \n"
//        "ldr pc, =_irq_handler              \n"
//        "ldr pc, =_fiq_handler              \n"
//
//        // These vectors and code are used for rom patching.
//        ".org 0x20                          \n"
//        ".code 16                           \n"
//        "_RPTV_0_START:                     \n"
//        "bx lr                              \n" // do nothing
//
//        ".org 0x60                          \n"
//        "_RPTV_1_START:                     \n"
//        "bx lr                              \n" // do nothing
//
//        ".org 0xa0                          \n"
//        "_RPTV_2_START:                     \n"
//        "bx lr                              \n" // do nothing
//
//        ".org 0xe0                          \n"
//        "_RPTV_3_START:                     \n"
//        "bx lr                              \n" // do nothing
//
//        ".org 0x120                         \n"
//        "ROM_var_start: .word 0             \n"
//        ".org 0x7ff                         \n"
//        "ROM_var_end:   .word 0             \n"
//        ".code 32                           \n"
//);
