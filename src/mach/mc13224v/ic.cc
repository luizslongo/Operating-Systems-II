// EPOS-- MC13224V IC Mediator Implementation

#include <mach/mc13224v/ic.h>

#include <machine.h>
#include <system/memory_map.h>

__BEGIN_SYS

MC13224V_IC::Interrupt_Handler MC13224V_IC::vector[11];

__END_SYS

__USING_SYS

extern "C" {

void _undefined_instruction()
{
    kout << "undefined instruction\n";
}

void _software_interrupt()
{
    kout << "software interrupt\n";
}

void _prefetch_abort()
{
    kout << "prefetch abort\n";
}

void _data_abort()
{
    kout << "data abort\n";
}

void _reserved()
{
    kout << "reserved\n";
}

void _irq_handler() {
    // call system handler
    MC13224V_IC::int_handler(CPU::in16(IO_Map<Machine>::ITC_NIPEND));
}

void _fiq_handler()
{
    kout << "fiq handler\n";
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
