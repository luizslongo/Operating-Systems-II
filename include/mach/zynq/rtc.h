#ifndef __panda_rtc_h
#define __panda_rtc_h

#include <rtc.h>

__BEGIN_SYS

class PandaBoard_RTC: public RTC_Common
{
public:
	PandaBoard_RTC() {}

    static Date date();
    static void date(const Date & d);

    static Second seconds_since_epoch() { 
        return 0;
    }
};

__END_SYS

#endif
