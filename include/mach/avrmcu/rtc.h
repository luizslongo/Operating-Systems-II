// EPOS-- AVRMCU_RTC Declarations

#ifndef __avrmcu_rtc_h
#define __avrmcu_rtc_h

#include <rtc.h>

__BEGIN_SYS

class AVRMCU_RTC: public RTC_Common
{
private:
    typedef Traits<AVRMCU_RTC> Traits;
    static const Type_Id TYPE = Type<AVRMCU_RTC>::TYPE;
    static const unsigned int EPOCH_DAYS = Traits::EPOCH_DAYS;

public:
    AVRMCU_RTC() {}

    Seconds get();
    void write(const Seconds & time);

    static int init(System_Info * si);
};

typedef AVRMCU_RTC RTC;

__END_SYS

#endif
