// EPOS-- ATMega128 ADC Mediator Declarations

#ifndef __atmega128_adc_h
#define __atmega128_adc_h

#include "../avr_common/adc.h"

__BEGIN_SYS

class ATMega128_ADC: public ADC_Common, private AVR_ADC
{
public:
    ATMega128_ADC() : AVR_ADC(SINGLE_ENDED_ADC0, SYSTEM_REF,
			      FREE_RUNNING_MODE, CLOCK >> 7) {}
    ATMega128_ADC(unsigned char channel, Hertz frequency) 
	: AVR_ADC(channel, SYSTEM_REF, FREE_RUNNING_MODE, frequency) {}
    ATMega128_ADC(unsigned char channel, unsigned char reference,
		  unsigned char trigger, Hertz frequency) 
	: AVR_ADC(channel, reference, trigger, frequency) {}

    void config(unsigned char channel, unsigned char reference,
		unsigned char trigger, Hertz frequency) {
	AVR_ADC::config(channel, reference, trigger, frequency);
    }
    void config(unsigned char * channel, unsigned char * reference,
		unsigned char * trigger, Hertz * frequency) {
	AVR_ADC::config(channel, reference, trigger, frequency);
    }

    int get() { return AVR_ADC::get(); }
    bool finished() { return AVR_ADC::finished(); }

    void enable() { return AVR_ADC::enable(); }
    void disable() { return AVR_ADC::disable(); }

    void reset() { return AVR_ADC::reset(); }
     
    static int init(System_Info * si) { return 0; }
};

__END_SYS

#endif
