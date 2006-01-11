// EPOS-- ATMega16 NIC Mediator Initialization

#include <system/kmalloc.h>
#include <mach/atmega16/machine.h>

__BEGIN_SYS

template <int unit>
inline static int call_init(System_Info * si)
{
    int status = Traits<ATMega16_NIC>::NICS::template Get<unit>::Result
	::init(unit, si);
    status |= call_init<unit + 1>(si);
    return status;
};

template <> 
inline static
int call_init<Traits<ATMega16_NIC>::NICS::Length>(System_Info * si) 
{
    return 0;
};

int ATMega16_NIC::init(System_Info * si)
{
    return call_init<0>(si);
}

__END_SYS
