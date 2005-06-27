// EPOS-- Mica2_Display Declarations

#ifndef __mica2_display_h
#define __mica2_display_h

#include <display.h>

__BEGIN_SYS

class Mica2_Display: public Display_Common
{
private:
    typedef Traits<Mica2_Display> Traits;
 //   static const Type_Id TYPE = Type<Mica2_Display>::TYPE;

    // Mica2_Display private imports, types and constants

public:
    Mica2_Display(){};
    ~Mica2_Display(){};
    void clear(){};
    void putc(char c){};
    void puts(const char * s){};
    int lines(){ return 0; };
    int columns(){ return 0; };
    void position(int * line, int * column){};
    void position(int line, int column){};

    static int init(System_Info *si);

private:
    // Mica2_Display implementation methods

private:
    // Mica2_Display attributes
    
};

typedef Mica2_Display Display;

__END_SYS

#endif

