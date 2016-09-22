// EPOS Cortex_M Mediator Initialization

#include <machine/cortex/machine.h>

__BEGIN_SYS

void Machine::init()
{
    db<Init, Machine>(TRC) << "Cortex::init()" << endl;

    Machine_Model::init();

    if(Traits<IC>::enabled)
        IC::init();
    if(Traits<Timer>::enabled)
        Timer::init();
    if(Traits<USB>::enabled)
        USB::init();
#ifndef __no_networking__
    if(Traits<NIC>::enabled)
        NIC::init();
#endif
}

__END_SYS
