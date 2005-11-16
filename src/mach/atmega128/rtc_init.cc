#include <mach/atmega128/rtc.h>

__BEGIN_SYS

int ATMega128_RTC::init(System_Info * si)
{
    db<ATMega128_RTC>(TRC) << "ATMega128_RTC::init()\n";

    return 0;
}

__END_SYS

