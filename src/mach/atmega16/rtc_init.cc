#include <mach/atmega16/rtc.h>

__BEGIN_SYS

int ATMega16_RTC::init(System_Info * si)
{
    db<ATMega16_RTC>(TRC) << "ATMega16_RTC::init()\n";

    return 0;
}

__END_SYS

