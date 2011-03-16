// EPOS-- MC13224V Mediator Initialization

#include <mach/mc13224v/machine.h>
#include <mach/mc13224v/battery.h>

__BEGIN_SYS

void MC13224V::init()
{
    if (Traits<MC13224V_Battery>::enabled)
        MC13224V_Battery::init();

    if (Traits<MC13224V_NIC>::enabled)
		MC13224V_NIC::init();
}

__END_SYS

