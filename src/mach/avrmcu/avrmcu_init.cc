// EPOS-- AVRMCU Initialization

#include <mach/avrmcu/avrmcu.h>

__BEGIN_SYS

int AVRMCU::init(System_Info * si){
  interrupt_vector = (Handler *)si->lmm.int_vec;
  return 0;
}        
    

__END_SYS
