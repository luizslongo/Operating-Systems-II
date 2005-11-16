// EPOS-- ATMega128 Implementation

#include <mach/atmega128/atmega128.h>

__BEGIN_SYS

ATMega128::int_handler * ATMega128::interrupt_vector[Traits<ATMega128>::INT_VEC_SIZE];

__END_SYS
