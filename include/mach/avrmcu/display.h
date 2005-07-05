// EPOS-- AVRMCU_Display Declarations

#ifndef __avrmcu_display_h
#define __avrmcu_display_h

#include <display.h>
#include "../common/serial_display.h"

__BEGIN_SYS

class AVRMCU_Display: public Serial_Display
{
private:
    typedef Traits<AVRMCU_Display> Traits;
    static const Type_Id TYPE = Type<AVRMCU_Display>::TYPE;

public:
    static int init(System_Info *si);
    
};

typedef AVRMCU_Display Display;

__END_SYS

#endif

