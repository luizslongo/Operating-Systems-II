
// EPOS-- PLASMA Mediator Implementation

#include <mach/plasma/machine.h>


////extern "C" { void _exit(int s); }

__BEGIN_SYS

// Class attributes
PLASMA::int_handler * PLASMA::_interrupt_vector[PLASMA::INT_VECTOR_SIZE];

void PLASMA::panic() {
        db<PLASMA>(ERR) << "PANIC!\n";
	CPU::int_disable();
	CPU::halt();
}

void PLASMA::int_no_handler(unsigned int interrupt) {
	db<PLASMA>(WRN) << "Intr " << interrupt << " occurred but had no handler associated !\n";
////	_exit(-1);
}

__END_SYS

