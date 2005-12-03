// EPOS-- RTC Mediator Common Package

#ifndef __rtc_h
#define __rtc_h

#include <system/config.h>

__BEGIN_SYS

class RTC_Common
{
protected:
    RTC_Common() {}

public:
    // The time (as defined by God Chronos)
    typedef unsigned long Microsecond;
    typedef unsigned long Second;

protected:
    static Second date2offset(unsigned int epoch_days,
			      unsigned int Y, unsigned int M, 
			      unsigned int D, unsigned int h,
			      unsigned int m, unsigned int s);
    static void offset2date(Second t,  unsigned int epoch_days,
			    unsigned int * Y, unsigned int * M,
			    unsigned int * D, unsigned int * h,
			    unsigned int * m, unsigned int * s);
};

__END_SYS

#include __HEADER_MACH(rtc)

#endif
