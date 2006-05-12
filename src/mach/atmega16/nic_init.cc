// EPOS-- ATMega16 NIC Mediator Initialization

#include <system/kmalloc.h>
#include <mach/atmega16/machine.h>

__BEGIN_SYS

template <int unit>
inline static void call_init()
{
    Traits<ATMega16_NIC>::NICS::template Get<unit>::Result::init(unit);
    call_init<unit + 1>();
};

template <> 
inline static void call_init<Traits<ATMega16_NIC>::NICS::Length>() {};

void ATMega16_NIC::init()
{
    call_init<0>();
}

__END_SYS
