// EPOS-- ATMega128 NIC Mediator Initialization

#include <mach/atmega128/nic.h>

__BEGIN_SYS

int ATMega128_NIC::init(System_Info * si)
{
    db<ATMega128_NIC>(TRC) << "ATMega128_NIC::init()\n";

    return 0;
}

__END_SYS
