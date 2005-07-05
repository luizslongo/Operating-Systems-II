// EPOS-- Common Declarations for AVR ICs

#ifndef __avr_ic_h
#define __avr_ic_h

#include <ic.h>

__BEGIN_SYS

class AVR_IC: public IC_Common
{

public:
    
    static void remap(unsigned char b) {}
    static Mask servicing() { return 0; }
    static void eoi() {};


};

__END_SYS

#endif
