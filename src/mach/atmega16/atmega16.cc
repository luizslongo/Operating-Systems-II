// EPOS-- ATMega16 Implementation

#include <mach/atmega16/atmega16.h>

__BEGIN_SYS

ATMega16::int_handler * ATMega16::interrupt_vector[Traits<ATMega16>::INT_VEC_SIZE];

__END_SYS
