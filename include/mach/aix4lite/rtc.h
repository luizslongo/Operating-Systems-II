// EPOS AIX4LITE_RTC Declarations

#ifndef __aix4lite_rtc_h
#define __aix4lite_rtc_h

#include <rtc.h>

__BEGIN_SYS

class AIX4LITE_RTC: public RTC_Common
{
private:
   static const unsigned int EPOCH_DAYS = 0;

public:
    AIX4LITE_RTC() { }

    static Date date();
    static void date(const Date & d);

    static Second seconds_since_epoch() { 
        return date().to_offset(EPOCH_DAYS); 
    }
};

__END_SYS

#endif
