#include <mach/integratorcp/ic.h>

__BEGIN_SYS

IntegratorCP_IC::Handler* IntegratorCP_IC::_vector[29] =
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
    CPU::Reg32 status = CPU::in32(PIC_IRQ_STATUS);
        
    // search for interrupt id
    Interrupt_Id id = 0;
    CPU::Reg32 s = status;
    while (s >>= 1) {
        ++id;
    };
    
    db<IC>(TRC) << "IC::handle_int() id="<<(int)id<<"\n";
    
    // clear flags
    CPU::out32(PIC_IRQ_ENABLECLR, status);
    // call handler
    _vector[id]();
    // reenable int
    CPU::out32(PIC_IRQ_ENABLESET,CPU::in32(PIC_IRQ_ENABLESET) | status);
}

void IntegratorCP_IC::default_handler() {
    db<IntegratorCP_IC>(TRC) << "IntegratorCP_IC::default_handler()\n";    
}

__END_SYS
