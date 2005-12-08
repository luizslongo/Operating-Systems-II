// EPOS-- ATMega128_Display Declarations

#ifndef __atmega128_display_h
#define __atmega128_display_h

#include <display.h>
//#include "../common/serial_display.h"

__BEGIN_SYS

class ATMega128_Display: public Serial_Display
{
private:
    typedef Traits<ATMega128_Display> Traits;
    static const Type_Id TYPE = Type<ATMega128_Display>::TYPE;

public:
    static int init(System_Info *si);
    
};

typedef ATMega128_Display Display;

__END_SYS

#endif

