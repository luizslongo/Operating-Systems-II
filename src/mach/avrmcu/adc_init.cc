// EPOS-- AVRMCU_ADC Initialization

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.

#include <mach/avrmcu/adc.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_ADC::init(System_Info * si)
{
    db<AVRMCU_ADC>(TRC) << "AT90S_ADC::init()\n";

    return 0;
}

__END_SYS
