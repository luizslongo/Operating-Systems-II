// EPOS-- ATMega128_ADC Declarations

#ifndef __atmega128_adc_h
#define __atmega128_adc_h

#include <adc.h>
#include "../common/avr_adc.h"

__BEGIN_SYS

class ATMega128_ADC: public AVR_ADC
{
private:
    typedef Traits<ATMega128_ADC> Traits;
    static const Type_Id TYPE = Type<ATMega128_ADC>::TYPE;

public:

    ATMega128_ADC() : AVR_ADC() { }

    ATMega128_ADC(unsigned char channel, unsigned char reference,
	      unsigned char trigger, Hertz frequency) 
	: AVR_ADC(channel,reference,trigger,frequency) { }

    static int init(System_Info *si);
    
};

typedef ATMega128_ADC ADC;

__END_SYS

#endif

