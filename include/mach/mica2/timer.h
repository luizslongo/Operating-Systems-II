// EPOS-- Mica2_Timer Declarations

#ifndef __mica2_timer_h
#define __mica2_timer_h

#include <timer.h>
#include "memory_map.h"
#include "../common/avr_timer.h"

__BEGIN_SYS

class Mica2_Timer: public ATmega128_Timer0
{
private:
    typedef Traits<Mica2_Timer> Traits;
    static const Type_Id TYPE = Type<Mica2_Timer>::TYPE;
    
public:

    Mica2_Timer(int unit = 0){};
    ~Mica2_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); }
    void frequency(const Hertz & f){ ATmega128_Timer0::frequency(f); }

    static int init(System_Info *si);
    
};

typedef Mica2_Timer Timer;

__END_SYS

#endif
