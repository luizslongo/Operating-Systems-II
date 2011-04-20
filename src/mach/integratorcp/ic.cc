#include <ic.h>

__BEGIN_SYS

IC::Interrupt_Handler IC::vector[29];

extern "C" void __epos_irq_handler()
{
    IC::handle_int();
}

extern "C" void __epos_fiq_handler(){

}

extern "C" void __epos_swi_handler(){

}
__END_SYS
