// EPOS-- Mica2_ADC Declarations

#ifndef __mica2_adc_h
#define __mica2_adc_h

#include <adc.h>
#include "../common/avr_adc.h"

__BEGIN_SYS

class Mica2_ADC: public AVR_ADC
{
private:
    typedef Traits<Mica2_ADC> Traits;
    static const Type_Id TYPE = Type<Mica2_ADC>::TYPE;

public:

    Mica2_ADC() : AVR_ADC() { }

    Mica2_ADC(unsigned char channel, unsigned char reference,
	      unsigned char trigger, Hertz frequency) 
	: AVR_ADC(channel,reference,trigger,frequency) { }

    static int init(System_Info *si);
    
};

typedef Mica2_ADC ADC;

__END_SYS

#endif

