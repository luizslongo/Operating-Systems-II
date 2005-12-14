// EPOS-- AVR8 TSC Mediator Initialization

#include <tsc.h>
#include <machine.h>

__BEGIN_SYS

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
