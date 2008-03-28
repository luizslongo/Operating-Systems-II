// EPOS-- ATMega16 Mediator Implementation

#include <mach/atmega16/machine.h>

__BEGIN_SYS

ATMega16::int_handler * ATMega16::_int_vector[INT_VECTOR_SIZE];

__END_SYS


__USING_SYS

extern "C" void __epos_call_handler(char offset);

extern "C" void __epos_call_handler(char offset) {
    Machine::int_handler * handler = Machine::int_vector(offset);

    if (handler != 0)
	handler(0);
}

extern "C" void __epos_free_init_mem();

extern "C" void __epos_free_init_mem() {
    
//    MMU::free(Memory_Map<ATMega128>::TOP,$
//	      Memory_Map<ATMega128>::MEM_SIZE - 
//	      Memory_Map<ATMega128>::TOP + 
//	      Memory_Map<ATMega128>::PHY_MEM);
    
}
