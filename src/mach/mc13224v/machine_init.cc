// EPOS-- MC13224V Mediator Implementation

#include <mach/mc13224v/machine.h>
#include <mach/mc13224v/buck_regulator.h>

__BEGIN_SYS

void MC13224V::init()
{
    if (Traits<MC13224V_NIC>::enabled)
    {
		MC13224V_NIC::init();
    }

    if (Traits<MC13224V>::BUCK)
        Buck_Regulator::enable();
}

__END_SYS

