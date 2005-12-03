// EPOS-- Clock Abstraction Declarations

#ifndef __timepiece_h
#define __timepiece_h

#include <system/config.h>
#include <rtc.h>

__BEGIN_SYS

class Clock
{
private:
    typedef Traits<Clock> Traits;
    static const Type_Id TYPE = Type<Clock>::TYPE;

    typedef RTC::Microsecond Microsecond;
    typedef RTC::Second Second;

public:
    Clock() {}

    Microsecond resolution() { return 1000000; }

    Second now() { RTC rtc; return rtc.get(); }

    static int init(System_Info * si);
};

__END_SYS

#endif
