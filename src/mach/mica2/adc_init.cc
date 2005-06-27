// EPOS-- Mica2_ADC Initialization

#include <mach/mica2/adc.h>

__BEGIN_SYS

// Class initialization
int Mica2_ADC::init(System_Info * si)
{
    db<Mica2_ADC>(TRC) << "Mica2_ADC::init()\n";
    
    // Defaults: Channel 0, System Reference, Free Running Mode, CLOCK/128
    admux(0x00);			// Channel 0, AREF
    sfior(ADATE);			// Free Running Mode
    adcsra(ADPS2 | ADPS1 | ADPS0);	// CLOCK/128	

    return 0;
}

__END_SYS
