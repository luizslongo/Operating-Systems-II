// EPOS AVR8 Initialization
//
// Author: root
// Documentation: $EPOS/doc/cpu			Date: 27 Jan 2004

#include <arch/avr8/cpu.h>

__BEGIN_SYS

// Class initialization
int AVR8::init(System_Info * si)
{
    db<AVR8>(TRC) << "AVR8::init()\n";

    return 0;
}

__END_SYS
