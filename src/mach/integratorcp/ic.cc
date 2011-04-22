#include <mach/integratorcp/ic.h>

__BEGIN_SYS

IntegratorCP_IC::Interrupt_Handler IntegratorCP_IC::vector[29] =
{ &IntegratorCP_IC::default_handler, &IntegratorCP_IC::default_handler,
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
&IntegratorCP_IC::default_handler };

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

void _fiq_handler()
{
    kout << "fiq handler\n";
}

void _irq_handler() {
    IntegratorCP_IC::handle_int();
}

};

void IntegratorCP_IC::handle_int() {
    CPU::Reg32 status;// = CPU::in32(PIC_FIQ_STATUS);
    //if (status == 0)
        status = CPU::in32(PIC_IRQ_STATUS);
        
    db<IntegratorCP_IC>(TRC) << "IC::handle_int() status="<<status<<"\n";
    
    switch (status) {
        case 1<<TS_PENINT: // touch screen
            (vector[TS_PENINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<TS_PENINT);
            break;
        case 1<<ETH_INT: // ethernet
            (vector[ETH_INT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<ETH_INT);
            break;
        case 1<<CPPLDINT: // secondary IC
            (vector[CPPLDINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<TS_PENINT);
            break;
        case 1<<AACIINT: // audio codec
            (vector[AACIINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<AACIINT);
            break;
        case 1<<MMCIINT1:
            (vector[MMCIINT1])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<MMCIINT1);
            break;
        case 1<<MMCIINT2:
            (vector[MMCIINT2])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<MMCIINT2);
            break;            
        case 1<<CLCDCINT:  // display
            (vector[CLCDCINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<CLCDCINT);
            break;
        case 1<<LM_LLINT1: // logic module
            (vector[LM_LLINT1])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<LM_LLINT1);
            break;
        case 1<<LM_LLINT0:
            (vector[LM_LLINT0])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<LM_LLINT0);
            break;            
        case 1<<RTCINT:
            (vector[RTCINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<RTCINT);
            break;            
        case 1<<TIMERINT2:
            vector[TIMERINT2]();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<TIMERINT2);
            break;            
        case 1<<TIMERINT1:
            vector[TIMERINT1]();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<TIMERINT1);
            break;        
        case 1<<TIMERINT0:
            vector[TIMERINT0]();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<TIMERINT0);
            break;            
        case 1<<MOUSEINT:
            (vector[MOUSEINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<MOUSEINT);
            break;            
        case 1<<KBDINT:
            (vector[KBDINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<KBDINT);
            break;        
        case 1<<UARTINT1:
            (vector[UARTINT1])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<UARTINT1);
            break;            
        case 1<<UARTINT0:
            (vector[UARTINT0])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<UARTINT0);
            break;            
        case 1<<SOFTINT:   
            (vector[SOFTINT])();
            CPU::out32(PIC_IRQ_ENABLECLR, 1<<SOFTINT);
            break;
    }
    
}

void IntegratorCP_IC::default_handler() {
    db<IntegratorCP_IC>(TRC) << "IntegratorCP_IC::default_handler()\n";    
}

__END_SYS
