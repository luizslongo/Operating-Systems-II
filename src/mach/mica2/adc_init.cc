// EPOS-- Mica2_ADC Initialization

#include <mach/mica2/adc.h>

__BEGIN_SYS

// Class initialization
int Mica2_ADC::init(System_Info * si)
{
    db<Mica2_ADC>(TRC) << "Mica2_ADC::init()\n";
    
    return 0;
}

__END_SYS
