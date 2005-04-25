// EPOS-- ADC Mediator Common Package

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __adc_h
#define __adc_h

#include <system/config.h>

__BEGIN_SYS

class ADC_Common
{

 public:

  typedef unsigned long Hertz;

  enum Channel {
    SINGLE_ENDED_ADC0 = 0x00,
    SINGLE_ENDED_ADC1 = 0x01,
    SINGLE_ENDED_ADC2 = 0x02,
    SINGLE_ENDED_ADC3 = 0x03,
    SINGLE_ENDED_ADC4 = 0x04,
    SINGLE_ENDED_ADC5 = 0x05,
    SINGLE_ENDED_ADC6 = 0x06,
    SINGLE_ENDED_ADC7 = 0x07
  };

  enum Reference {
    SYSTEM_REFERENCE,
    EXTERNAL_REFERENCE,
    INTERNAL_REFERENCE
  };

  enum Trigger {
    FREE_RUNNING_MODE,
    SINGLE_CONVERSION_MODE
  };

};


__END_SYS

#include __HEADER_MACH(adc)

#endif
