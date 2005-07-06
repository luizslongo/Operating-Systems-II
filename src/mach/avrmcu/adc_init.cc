// EPOS-- AVRMCU_ADC Initialization

#include <mach/avrmcu/adc.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_ADC::init(System_Info * si)
{
    db<AVRMCU_ADC>(TRC) << "AT90S_ADC::init()\n";
    
    return 0;
}

__END_SYS
