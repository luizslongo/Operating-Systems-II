// EPOS-- ATMega128_RTC Declarations

#ifndef __atmega128_rtc_h
#define __atmega128_rtc_h

#include <rtc.h>

__BEGIN_SYS

class ATMega128_RTC: public RTC_Common
{
private:
    typedef Traits<ATMega128_RTC> Traits;
    static const Type_Id TYPE = Type<ATMega128_RTC>::TYPE;
    static const unsigned int EPOCH_DAYS = Traits::EPOCH_DAYS;

public:
    ATMega128_RTC() {}

    Second get();
    void write(const Second & time);

    static int init(System_Info * si);
};

typedef ATMega128_RTC RTC;

__END_SYS

#endif
