// EPOS-- PC NIC Mediator Initialization

#include <mach/pc/nic.h>

__BEGIN_SYS

template <int unit>
inline static void call_init()
{
    Traits<PC_NIC>::NICS::template Get<unit>::Result::init(unit);
    call_init<unit + 1>();
};

template <>
inline static void call_init<Traits<PC_NIC>::NICS::Length>() 
{
};

void PC_NIC::init()
{
    call_init<0>();
}

__END_SYS
