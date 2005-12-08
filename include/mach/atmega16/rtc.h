// EPOS-- ATMega16_RTC Declarations

#ifndef __atmega16_rtc_h
#define __atmega16_rtc_h

#include <rtc.h>

__BEGIN_SYS

class ATMega16_RTC: public RTC_Common
{
private:
    typedef Traits<ATMega16_RTC> Traits;
    static const Type_Id TYPE = Type<ATMega16_RTC>::TYPE;
    static const unsigned int EPOCH_DAYS = Traits::EPOCH_DAYS;

public:
    ATMega16_RTC() {}

    Second get();
    void write(const Second & time);

    static int init(System_Info * si);
};

typedef ATMega16_RTC RTC;

__END_SYS

#endif
