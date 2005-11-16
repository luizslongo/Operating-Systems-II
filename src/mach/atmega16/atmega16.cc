// EPOS-- ATMega16 Implementation

#include <mach/atmega16/atmega16.h>

__BEGIN_SYS

Handler::Function * ATMega16::interrupt_vector[Traits<ATMega16>::INT_VEC_SIZE];

__END_SYS
