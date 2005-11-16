// EPOS-- ATMega128_Timer Declarations

#ifndef __atmega128_timer_h
#define __atmega128_timer_h

#include <timer.h>
#include "memory_map.h"
#include "../common/avr_timer.h"

__BEGIN_SYS

class ATMega128_Timer: public ATmega128_Timer0
{
private:
    typedef Traits<ATMega128_Timer> Traits;
    static const Type_Id TYPE = Type<ATMega128_Timer>::TYPE;
    
public:

    ATMega128_Timer(int unit = 0){};
    ~ATMega128_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); }
    void frequency(const Hertz & f){ ATmega128_Timer0::frequency(f); }

    static int init(System_Info *si);
    
};

typedef ATMega128_Timer Timer;

__END_SYS

#endif
