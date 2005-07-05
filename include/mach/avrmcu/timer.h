// EPOS-- AVRMCU_Timer Declarations

#ifndef __avrmcu_timer_h
#define __avrmcu_timer_h

#include <timer.h>
#include "memory_map.h"
#include "../common/avr_timer.h"

__BEGIN_SYS

class AVRMCU_Timer: public ATmega16_Timer0
{
private:
    typedef Traits<AVRMCU_Timer> Traits;
    static const Type_Id TYPE = Type<AVRMCU_Timer>::TYPE;
    
public:

    AVRMCU_Timer(int unit = 0){};
    ~AVRMCU_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); }
    void frequency(const Hertz & f){ ATmega16_Timer0::frequency(f); }

    static int init(System_Info *si);
    
};

typedef AVRMCU_Timer Timer;

__END_SYS

#endif
