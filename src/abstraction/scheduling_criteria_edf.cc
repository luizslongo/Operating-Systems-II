// EPOS EDF Periodic Thread Abstraction Implementation

#include <scheduler.h>
#include <alarm.h>

__BEGIN_SYS

// Class attributes

volatile RTC::Microsecond Scheduling_Criteria::EDF::_last_release;

__END_SYS
