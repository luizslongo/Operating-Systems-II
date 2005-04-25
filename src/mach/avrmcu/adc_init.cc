// EPOS-- AVRMCU_ADC Initialization

#include <mach/avrmcu/adc.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_ADC::init(System_Info * si)
{
    db<AVRMCU_ADC>(TRC) << "AT90S_ADC::init()\n";
    
    // Defaults: Channel 0, System Reference, Free Running Mode, CLOCK/128
    admux(0x00);			// Channel 0, AREF
    sfior(ADATE);			// Free Running Mode
    adcsra(ADPS2 | ADPS1 | ADPS0);	// CLOCK/128	

    return 0;
}

__END_SYS
