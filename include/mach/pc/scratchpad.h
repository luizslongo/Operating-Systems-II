// EPOS PC Scratchpad Memory Mediator Declarations

#include <scratchpad.h>

#ifndef __pc_scratchpad_h
#define __pc_scratchpad_h

__BEGIN_SYS

class PC_Scratchpad: public Scratchpad_Common
{
private:
    static const unsigned int ADDRESS = Traits<PC_Scratchpad>::ADDRESS;
    static const unsigned int SIZE = Traits<PC_Scratchpad>::SIZE;

public:
    PC_Scratchpad() {}

    static void init();
};

__END_SYS

#endif
