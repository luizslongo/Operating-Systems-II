#include <mach/zynq/ic.h>

__BEGIN_SYS

PandaBoard_IC::Interrupt_Handler PandaBoard_IC::_vector[PandaBoard_IC::INTS] =
{ 
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler, 
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler, 
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler, 
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler, 
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler,
    &PandaBoard_IC::default_handler, &PandaBoard_IC::default_handler
};

CPU::Reg32 PandaBoard_IC::_mask = 0;

void PandaBoard_IC::int_handler() {
    //CPU::Reg32 status = CPU::in32(PIC_IRQ_STATUS);
    kout << "IC::int_handler()\n"; 
    
    //disable_cpu_interface();
    //disable_distributor();
    
    //ICCIAR also has the CPU id for SGIs interrupts
    unsigned int icciar_value = CPU::in32(GIC_PROC_INTERFACE + ICCIAR);
    Interrupt_Id id = icciar_value & INTERRUPT_MASK; //0x3FF bits 0 to 9
    
    // search for interrupt id
    /*Interrupt_Id id = 0;
    CPU::Reg32 s = status;
    while (s >>= 1) {
        ++id;
    };*/
    
    db<IC>(TRC) << "IC::int_handler()\n";    
   
    // call handler
    _vector[id](id);
    //db<IC>(TRC) << "return from handler\n";
    
    //For compatility, arm recommends to preserve the entire register value
    //read from the ICCIAR when acknowledging an interrupt
    CPU::out32(GIC_PROC_INTERFACE + ICCEOI, icciar_value);
    
    //enable_cpu_interface();
    //enable_distributor();
}

void PandaBoard_IC::default_handler(Interrupt_Id id) {
    db<PandaBoard_IC>(INF) << "PandaBoard_IC::default_handler(id="<<id<<")\n";
    kout << "PandaBoard_IC::default_handler(id=" << id << ")\n";
}


__END_SYS
