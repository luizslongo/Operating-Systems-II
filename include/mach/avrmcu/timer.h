// EPOS-- AVRMCU_Timer Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __avrmcu_timer_h
#define __avrmcu_timer_h

#include <timer.h>

__BEGIN_SYS

class AVRMCU_Timer: public Timer_Common
{
private:
    typedef Traits<AVRMCU_Timer> Traits;
    static const Type_Id TYPE = Type<AVRMCU_Timer>::TYPE;

    // AVRMCU_Timer private imports, types and constants
    
public:

    // This should be informed by the MCU
    static const int CLOCK = 7812;
    
// Timer Registers
    enum {
    	OCR0 = 0x3c,
	TIMSK = 0x39,
	TIFR = 0x38,
    	TCCR0 = 0x33,
	TCNT0 = 0x32,
	SFIOR = 0x30,
	TCCR1A = 0x2f,
	TCCR1B = 0x2e,
	TCNT1H = 0x2d,
	TCNT1L = 0x2c,
	OCR1AH = 0x2b,
	OCR1AL = 0x2a,
	OCR1BH = 0x29,
	OCR1BL = 0x28,
	ICR1H = 0x27,
	ICR1L = 0x26,
	TCCR2 = 0x25,
	TCNT2 = 0x24,
	OCR2 = 0x23,
	ASSR = 0x22
    };
    
    // Timer Register Bits
    enum {
    	// TIMSK
	OCIE2 = 0x80,
	TOIE2 = 0x40,
	TICIE1 = 0x20,
	OCIE1A = 0x10,
	OCIE1B = 0x08,
	TOIE1 = 0x04,
	OCIE0 = 0x02,
	TOIE0 = 0x01,     
	// TIFR
	OCF2 = 0x80,
	TOV2 = 0x40,
	ICF1 = 0x20,
	OCF1A = 0x10,
	OCF1B = 0x08,
	TOV1 = 0x04,
	OCF0 = 0x02,
	TOV0 = 0x01,	
    	// TCCR0
	FOC0 = 0x80,
	WGM00 = 0x40,
	COM01 = 0x20,
	COM00 = 0x10,
	WGM01 = 0x08,
	CS02 = 0x04,
	CS01 = 0x02,
	CS00 = 0x01, 	
    	// TCCR1A
    	COM1A1 = 0x80,
	COM1A0 = 0x40,
	COM1B1 = 0x20,
	COM1B0 = 0x10,
	FOC1A = 0x08,
	FOC1B = 0x04,
	WGM11 = 0x02,
	WGM10 = 0x01,
	// TCCR1B
	ICNC1 = 0x80,
	ICES1 = 0x40,
	WGM13 = 0x10,
	WGM12 = 0x08,
	CS12 = 0x04,
	CS11 = 0x02,
	CS10 = 0x01,
	// TCCE2
	FOC2 = 0x80,
	WGM20 = 0x40,
	COM21 = 0x20,
	COM20 = 0x10,
	WGM21 = 0x08,
	CS22 = 0x04,
	CS21 = 0x02,
	CS20 = 0x01, 	
	// ASSR
	AS2 = 0x08,
	TCN2UB = 0x04,
	OCR2UB = 0x02,
	TCR2UB = 0x01, 
	// SFIOR
	PSR10 = 0x01
    };
    
    // Register Settings
    enum {
    	TIMER0_PRESCALE_1 = CS00,
	TIMER0_PRESCALE_8 = CS01,
	TIMER0_PRESCALE_64 = CS01 | CS00,
	TIMER0_PRESCALE_256 = CS02,
	TIMER0_PRESCALE_1024 = CS02 | CS00,
    };

    AVRMCU_Timer(int unit = 0){};
    ~AVRMCU_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); };
    
    void frequency(const unsigned short & f){
    	unsigned short count = freq2cnt(f);
	Timer_Common::frequency(cnt2freq(count));
	
	AVR8::out8(TCCR0,TIMER0_PRESCALE_1024);
	AVR8::out8(OCR0,count);
    };

    void enable(){
    	// This should be done by the IC!!!
	AVR8::out8(TIMSK,OCIE0);
	
    };
    void disable(){
        // This should be done by the IC!!!
    	AVR8::out8(TIMSK,0);
    };

    static int init(System_Info *si);

private:

    static Hertz cnt2freq(unsigned short c) { return CLOCK / c; }
    static unsigned short freq2cnt(const Hertz & f) { return CLOCK / f; }

    
};

__END_SYS

#endif
