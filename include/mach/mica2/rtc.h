// EPOS-- Mica2_RTC Declarations

#ifndef __mica2_rtc_h
#define __mica2_rtc_h

#include <rtc.h>

__BEGIN_SYS

class Mica2_RTC: public RTC_Common
{
private:
    typedef Traits<Mica2_RTC> Traits;
    static const Type_Id TYPE = Type<Mica2_RTC>::TYPE;
    static const unsigned int EPOCH_DAYS = Traits::EPOCH_DAYS;

public:
    Mica2_RTC() {}

    Seconds get();
    void write(const Seconds & time);

    static int init(System_Info * si);
};

typedef Mica2_RTC RTC;

__END_SYS

#endif
