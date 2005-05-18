// EPOS-- AVRMCU_Display Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __avrmcu_display_h
#define __avrmcu_display_h

#include <display.h>

__BEGIN_SYS

class AVRMCU_Display: public Display_Common
{
private:
    typedef Traits<AVRMCU_Display> Traits;
 //   static const Type_Id TYPE = Type<AVRMCU_Display>::TYPE;

    // AVRMCU_Display private imports, types and constants

public:
    AVRMCU_Display(){};
    ~AVRMCU_Display(){};
    void clear(){};
    void putc(char c){};
    void puts(const char * s){};
    int lines(){ return 0; };
    int columns(){ return 0; };
    void position(int * line, int * column){};
    void position(int line, int column){};

    static int init(System_Info *si);

private:
    // AVRMCU_Display implementation methods

private:
    // AVRMCU_Display attributes
    
};

typedef AVRMCU_Display Display;

__END_SYS

#endif

