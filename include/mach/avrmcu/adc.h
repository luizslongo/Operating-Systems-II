// EPOS-- AVRMCU ADC Mediator

#ifndef __avrmcu_adc_h
#define __avrmcu_adc_h

#include <adc.h>

__BEGIN_SYS

class AVRMCU_ADC: public ADC_Common
{
 private:

    // This should be informed by the MCU
    static const long long CLOCK = Traits<AVR8>::CLOCK;

    typedef Traits<AVRMCU_ADC> _Traits;
    static const Type_Id TYPE = Type<AVRMCU_ADC>::TYPE;

    // ADC IO Registers
    enum {
        ADMUX 	= 0x07,
        ADCSRA 	= 0x06,
        ADCH 	= 0x05,
        ADCL 	= 0x04,
        SFIOR 	= 0x30
    };

    //UART IO Register Bits
    enum {
        // ADMUX
        REFS1 = 0x80,
        REFS0 = 0x40,
        ADLAR = 0x20,
        MUX4 = 0x10,
        MUX3 = 0x08,
        MUX2 = 0x04,
        MUX1 = 0x02,
        MUX0 = 0x01,
        // ADCSRA
        ADEN = 0x80,
        ADSC = 0x40,
        ADATE = 0x20,
        ADIF = 0x10,
        ADIE = 0x08,
        ADPS2 = 0x04,
        ADPS1 = 0x02,
        ADPS0 = 0x01,
        // SFIOR
        ADTS2 = 0x80,
        ADTS1 = 0x40,
        ADTS0 = 0x20,
    };


 public:

    AVRMCU_ADC()    { /* Default Initialization is up to init */ }

    AVRMCU_ADC(Channel c,Reference r, Trigger t, Hertz f) {
	channel(c);
	reference(r);
	trigger(t);
	frequency(f);
    }
    
    ~AVRMCU_ADC();

    int read(){
        if (data_ready() == 1)
	    return adchl();
        return -1;
    }


    int data_ready(){
        if (adcsra() & ADIF)
            return 1;
        return -1;
    }

    int start(){
    	adcsra(adcsra() | ADSC | ADEN);
        return 1;
    }

    int stop(){
        adcsra(adcsra() & ~ADSC & ~ADEN);
        return 1;
    }

    int trigger(Trigger t){
        if (t == FREE_RUNNING_MODE) {
	    sfior(sfior() & ~ADTS2 & ~ADTS1 & ~ADTS0);
	    adcsra(adcsra() | ADATE);
        } else if (t == SINGLE_CONVERSION_MODE) {
	    adcsra(adcsra() & ~ADATE);
        }
        return 1;
    }

    int channel(Channel c){
    	admux(c | (admux() & ~MUX4 & ~MUX3 & ~MUX2 & ~MUX1 & ~MUX0));
        return 1;
    }

    int reference(Reference r)
    {
        unsigned char mask = admux();
        switch (r){
            case SYSTEM_REFERENCE:   mask &= ~REFS1 & ~REFS0; 		break; 
            case EXTERNAL_REFERENCE: mask = (mask & ~REFS1) | REFS0;    break;
            case INTERNAL_REFERENCE: mask |= REFS1 | REFS0;    		break;
        }
        admux(mask);
        return 1;
    }

    Hertz frequency(Hertz f)
    {
        unsigned int div = CLOCK/f;
        unsigned int ps;
        // there MUST be a better way to do this!
        if (div >= 128) {
            ps = ADPS2|ADPS1|ADPS0;
            div = 128;
        } else if (div >= 64) {
            ps = ADPS2|ADPS1;
            div = 64;
        } else if (div >= 32) {
            ps = ADPS2|ADPS0;
            div = 32;
        } else if (div >= 16) {
            ps = ADPS2;
            div = 16;
        } else if (div >= 8) {
            ps = ADPS1 | ADPS0;
            div = 8;
        } else if (div >= 4) {
            ps = ADPS1;
            div = 4;
        } else {
            ps = 0;
            div = 2;
        }
        ps |= (adcsra() & ~ADPS2 & ~ADPS1 & ~ADPS0);
        adcsra(ps);
        return (Hertz)(CLOCK/div);
    }
    
    static int init(System_Info *si);
 
private:

    typedef AVR8::Reg8 Reg8;
    typedef AVR8::Reg16 Reg16;

    static Reg8 admux(){
	return AVR8::in8(ADMUX);
    }
    
    static void admux(Reg8 value){
	AVR8::out8(ADMUX,value);
    }   
    
    static Reg8 adcsra(){
	return AVR8::in8(ADCSRA);
    }
    
    static void adcsra(Reg8 value){
	AVR8::out8(ADCSRA,value);
    }       
    
    static Reg8 sfior(){
	return AVR8::in8(SFIOR);
    }
    
    static void sfior(Reg8 value){
	AVR8::out8(SFIOR,value);
    }     
    
    static Reg16 adchl(){
	return AVR8::in16(ADCL);
    }
    
    static void adchl(Reg16 value){
	AVR8::out16(ADCL,value);
    }           
  

};

typedef AVRMCU_ADC ADC;

__END_SYS


#endif
