// EPOS-- AVRMCU_RTC Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


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
    ~AVRMCU_RTC() {}

    Seconds read();
    void write(const Seconds & time);

    static int init(System_Info *si){ return 0; }
};

__END_SYS

#endif
