// EPOS-- AVR8_TSC Declarations
// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __avr8_tsc_h
#define __avr8_tsc_h

#include <tsc.h>

__BEGIN_SYS

class AVR8_TSC: public TSC_Common
{
private:
    typedef Traits<AVR8_TSC> Traits;
    static const Type_Id TYPE = Type<AVR8_TSC>::TYPE;

public:
    AVR8_TSC(){};
    ~AVR8_TSC(){};
    
    static Hertz frequency(){ return 0; };
    static Time_Stamp time_stamp(){ return 0; };

    static int init(System_Info *si);
};

__END_SYS

#endif
