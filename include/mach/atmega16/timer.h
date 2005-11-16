// EPOS-- ATMega16_Timer Declarations

#ifndef __atmega16_timer_h
#define __atmega16_timer_h

#include <timer.h>
#include "memory_map.h"
#include "../common/avr_timer.h"

__BEGIN_SYS

class ATMega16_Timer: public ATmega16_Timer0
{
private:
    typedef Traits<ATMega16_Timer> Traits;
    static const Type_Id TYPE = Type<ATMega16_Timer>::TYPE;
    
public:

    ATMega16_Timer(int unit = 0){};
    ~ATMega16_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); }
    void frequency(const Hertz & f){ ATmega16_Timer0::frequency(f); }

    static int init(System_Info *si);
    
};

typedef ATMega16_Timer Timer;

__END_SYS

#endif
