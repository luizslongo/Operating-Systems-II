// EPOS AVR8_TSC Initialization
//
// Author: fauze
// Documentation: $EPOS/doc/tsc			Date: 27 Jan 2004

#include <arch/avr8/tsc.h>

__BEGIN_SYS

// Class initialization
int AVR8_TSC::init(System_Info * si)
{
    db<AVR8_TSC>(TRC) << "AVR8_TSC::init()\n";

//    tccr1b(CS10);
   
    Machine::int_vector(IC::IRQ_TSC, timer_handler);
    
//     IC ic;
//     ic.enable(IC::TIMER1_OVF);

    return 0;
}

__END_SYS
