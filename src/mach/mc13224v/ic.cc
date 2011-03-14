// EPOS-- MC13224V IC Mediator Implementation

#include <mach/mc13224v/ic.h>

__BEGIN_SYS

__END_SYS

__USING_SYS

#include <cpu.h>
#include <system/memory_map.h>

typedef IO_Map<Machine> IO;


IC::Interrupt_Handler IC::vector[11];

extern "C" void __epos_irq_handler(){
	IC::int_handler(CPU::in16(IO::ITC_NIPEND));
}

extern "C" void __epos_fiq_handler(){

}

extern "C" void __epos_swi_handler(){

}
