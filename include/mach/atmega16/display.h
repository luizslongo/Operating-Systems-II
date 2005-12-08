// EPOS-- ATMega16_Display Declarations

#ifndef __atmega16_display_h
#define __atmega16_display_h

#include <display.h>
//#include "../common/serial_display.h"

__BEGIN_SYS

class ATMega16_Display: public Serial_Display
{
private:
    typedef Traits<ATMega16_Display> Traits;
    static const Type_Id TYPE = Type<ATMega16_Display>::TYPE;

public:
    static int init(System_Info *si);
    
};

typedef ATMega16_Display Display;

__END_SYS

#endif

