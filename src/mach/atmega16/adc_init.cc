// EPOS-- ATMega16_ADC Initialization

#include <mach/atmega16/adc.h>

__BEGIN_SYS

// Class initialization
int ATMega16_ADC::init(System_Info * si)
{
    db<ATMega16_ADC>(TRC) << "ATMega16_ADC::init()\n";
    
    return 0;
}

__END_SYS
