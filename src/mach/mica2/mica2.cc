// EPOS-- Mica2 Implementation

#include <mach/mica2/mica2.h>

__BEGIN_SYS

Handler::Function * Mica2::interrupt_vector[Traits<Mica2>::INT_VEC_SIZE];

__END_SYS
