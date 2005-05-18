#include <mach/avrmcu/rtc.h>

__BEGIN_SYS

int AVRMCU_RTC::init(System_Info * si)
{
    db<AVRMCU_RTC>(TRC) << "AT90S_RTC::init()\n";

    return 0;
}

__END_SYS

