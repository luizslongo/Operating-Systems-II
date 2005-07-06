// EPOS-- Common Declarations for AVR ADCs

#ifndef __avr_adc_h
#define __avr_adc_h

#include <adc.h>

__BEGIN_SYS

class AVR_ADC: public ADC_Common
{
private:

    typedef IO_Map<Machine> IO;

    enum {
        // ADMUX
        REFS1 = 7,
        REFS0 = 6,
        ADLAR = 5,
        MUX4  = 4,
        MUX3  = 3,
        MUX2  = 2,
        MUX1  = 1,
        MUX0  = 0,
        // ADCSRA
        ADEN  = 7,
        ADSC  = 6,
        ADFR  = 5,
        ADIF  = 4,
        ADIE  = 3,
        ADPS2 = 2,
        ADPS1 = 1,
        ADPS0 = 0,
    };

public:
     
    static const unsigned long CLOCK = Traits<CPU>::CLOCK;

    AVR_ADC() {
	config(SINGLE_ENDED_ADC0,SYSTEM_REF,
	       FREE_RUNNING_MODE,CLOCK >> 7);
    }

    AVR_ADC(unsigned char channel, unsigned char reference,
	    unsigned char trigger, Hertz frequency) {
	config(channel,reference,trigger,frequency);
    }

    ~AVR_ADC(){ 
	admux(0);
	adcsra(0);
    }

    void config(unsigned char channel, unsigned char reference,
		unsigned char trigger, Hertz frequency) {

	admux((channel << MUX0) | (reference << REFS0));

	unsigned char ps = 7;
	while(((CLOCK >> ps) < frequency) && (ps > 0)) ps--;

	adcsra((trigger << ADFR) | (ps << ADPS0));

    }

    void config(unsigned char * channel, unsigned char * reference,
		unsigned char * trigger, Hertz * frequency) {

	unsigned char mux = admux();
	*channel = (mux >> MUX0) & 0x07;
	*reference = (mux >> REFS0) & 0x03;
	
	unsigned char sra = adcsra();
	*trigger = (sra >> ADFR) & 0x01;
	*frequency = CLOCK << ((sra >> ADPS0) & 0x07);

    }

    void enable() {
	adcsra(adcsra() | (1 << ADEN) | (1 << ADSC));
    }

    void disable() {
	adcsra(adcsra() & ~(1 << ADEN) & ~(1 << ADSC));
    }

    void reset() { }

    void int_enable() {
	adcsra(adcsra() | (1 << ADIE));
    }

    void int_disable() {
	adcsra(adcsra() & ~(1 << ADIE));
    }

    bool adc_complete() { return (adcsra() & (1 << ADIF)); }

    int get() { while (!adc_complete()); return adchl();  }


private:

    typedef AVR8::Reg8 Reg8;
    typedef AVR8::Reg16 Reg16;

    static Reg8 admux(){ return AVR8::in8(IO::ADMUX); }
    static void admux(Reg8 value){ AVR8::out8(IO::ADMUX,value); }   
    static Reg8 adcsra(){ return AVR8::in8(IO::ADCSRA); }
    static void adcsra(Reg8 value){ AVR8::out8(IO::ADCSRA,value); }       
    static Reg16 adchl(){ return AVR8::in16(IO::ADCL); }
    static void adchl(Reg16 value){ AVR8::out16(IO::ADCL,value); }   

};

__END_SYS

#endif
