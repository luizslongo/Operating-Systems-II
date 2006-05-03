// EPOS-- ATMega128 Mediator Implementation

#include <mach/atmega128/machine.h>

__BEGIN_SYS

ATMega128::int_handler * ATMega128::_int_vector[INT_VECTOR_SIZE];

__END_SYS


__USING_SYS

extern "C" void __epos_call_handler(char offset);

extern "C" void __epos_call_handler(char offset) {
    Machine::int_handler * handler = Machine::int_vector(offset);

    if (handler != 0)
	handler(0);
}

