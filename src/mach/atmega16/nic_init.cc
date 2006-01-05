// EPOS-- ATMega16 NIC Mediator Initialization

#include <mach/atmega16/nic.h>

__BEGIN_SYS

int ATMega16_NIC::init(System_Info * si)
{
    db<Init, ATMega16_NIC>(TRC) << "ATMega16_NIC::init()\n";

    return 0;
}

__END_SYS
