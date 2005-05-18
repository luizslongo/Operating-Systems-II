// EPOS-- AVRMCU Implementation

#include <mach/avrmcu/avrmcu.h>

__BEGIN_SYS

Handler::Function * AVRMCU::interrupt_vector[Traits<AVRMCU>::INT_VEC_SIZE];

__END_SYS
