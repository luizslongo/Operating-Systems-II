// EPOS-- ATMega16 Radio (CC1000) NIC Mediator Initialization

#include <system/kmalloc.h>
#include <mach/atmega16/atmega16.h>
#include <mach/atmega16/radio.h>

__BEGIN_SYS

int Radio::init(unsigned int unit, System_Info * si)
{
    db<Init, Radio>(TRC) << "Radio::init(unit=" << unit << ")\n";

    return 0;
}

__END_SYS
