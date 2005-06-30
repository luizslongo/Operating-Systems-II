// EPOS-- Mica2_Display Declarations

#ifndef __mica2_display_h
#define __mica2_display_h

#include <display.h>
#include "../common/serial_display.h"

__BEGIN_SYS

class Mica2_Display: public Serial_Display
{
private:
    typedef Traits<Mica2_Display> Traits;
    static const Type_Id TYPE = Type<Mica2_Display>::TYPE;

public:
    static int init(System_Info *si);
    
};

typedef Mica2_Display Display;

__END_SYS

#endif

