// EPOS-- ATMega128_ADC Initialization

#include <mach/atmega128/adc.h>

__BEGIN_SYS

// Class initialization
int ATMega128_ADC::init(System_Info * si)
{
    db<ATMega128_ADC>(TRC) << "ATMega128_ADC::init()\n";
    
    return 0;
}

__END_SYS
