// EPOS-- PC RTC Mediator Initialization

#include <mach/pc/rtc.h>

__BEGIN_SYS

int PC_RTC::init(System_Info * si)
{
    db<PC_RTC>(TRC) << "PC_RTC::init()\n";

    return 0;
}

__END_SYS
