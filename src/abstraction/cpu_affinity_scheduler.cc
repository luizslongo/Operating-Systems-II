// EPOS CPU Affinity Scheduler Abstraction Implementation

#include <scheduler.h>

__BEGIN_SYS

// Class attributes
volatile unsigned int Scheduling_Criteria::CPU_Affinity::_next_cpu = 1;

__END_SYS
