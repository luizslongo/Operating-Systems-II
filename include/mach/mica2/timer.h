// EPOS-- Mica2_Timer Declarations

#ifndef __mica2_timer_h
#define __mica2_timer_h

#include <timer.h>

__BEGIN_SYS

class Mica2_Timer: public Timer_Common
{
private:
    typedef Traits<Mica2_Timer> Traits;
    static const Type_Id TYPE = Type<Mica2_Timer>::TYPE;
    typedef IO_Map<Machine> IO;

    // Mica2_Timer private imports, types and constants
    
public:

    // This should be informed by the MCU
    static const unsigned int CLOCK = Traits<CPU>::CLOCK / 1024;  // 7200 Hz
    
// Timer Registers
/*    enum {
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
	};*/
    
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
	TCN0UB = 0x04,
	OCR0UB = 0x02,
	TCR0UB = 0x01, 
	// SFIOR
	PSR10 = 0x01
    };
    
    // Register Settings
    enum {
    	TIMER0_PRESCALE_1 = CS00,
	TIMER0_PRESCALE_8 = CS01,
	TIMER0_PRESCALE_32 = CS01 | CS00,
	TIMER0_PRESCALE_64 = CS02,
	TIMER0_PRESCALE_128 = CS02 | CS00,
	TIMER0_PRESCALE_256 = CS02 | CS01,
	TIMER0_PRESCALE_1024 = CS02 | CS01 | CS00
    };

    Mica2_Timer(int unit = 0){};
    ~Mica2_Timer(){};

    Hertz frequency(){ return Timer_Common::frequency(); }
    
    // Frequency for this Timer must be between 28Hz and 7200Hz
    void frequency(const unsigned short & f){
    	unsigned short count = freq2cnt(f);
	Timer_Common::frequency(cnt2freq(count));
	AVR8::out8(IO::OCR0,count);
	AVR8::out8(IO::TCCR0,(WGM01 & ~WGM00 | TIMER0_PRESCALE_1024));
    };

    void enable(){ IC::enable(IC::TIMER0_COMP); }
    void disable(){ IC::disable(IC::TIMER0_COMP); }

    static int init(System_Info *si);

private:

    static Hertz cnt2freq(unsigned short c) { return CLOCK / c; }
    static unsigned short freq2cnt(const Hertz & f) { return CLOCK / f; }

    
};

typedef Mica2_Timer Timer;

__END_SYS

#endif
