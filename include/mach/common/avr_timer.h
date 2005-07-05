// EPOS-- Common Declarations for AVR Timers

#ifndef __avr_timer_h
#define __avr_timer_h

#include <timer.h>
#include <cpu.h>

#include <uart.h>

__BEGIN_SYS

class AVR_Timer: public Timer_Common {

protected:

    typedef IO_Map<Machine> IO;

    // Bit Definitions
    enum {
	// TIMSK
	OCIE2  = 0x80,
	TOIE2  = 0x40,
	TICIE1 = 0x20,
	OCIE1A = 0x10,
	OCIE1B = 0x08,
	TOIE1  = 0x04,
	OCIE0  = 0x02,
	TOIE0  = 0x01,
	// ETIMSK
	TICIE3 = 0x20,
	OCIE3A = 0x10,
	OCIE3B = 0x08,
	TOIE3  = 0x04,
	OCIE3C = 0x02,
	OCIE1C = 0x01,
	// TIFR
	OCF2   = 0x80,
	TOV2   = 0x40,
	ICF1   = 0x20,
	OCF1A  = 0x10,
	OCF1B  = 0x08,
	TOV1   = 0x04,
	OCF0   = 0x02,
	TOV0   = 0x01,
	// ETIFR
	ICF3   = 0x20,
	OCF3A  = 0x10,
	OCF3B  = 0x08,
	TOV3   = 0x04,
	OCF3C  = 0x02,
	OCF1C  = 0x01,
	// SFIOR
	TSM    = 0x80,
	ACME   = 0x08,
	PUD    = 0x04,
	PSR0   = 0x02,
	PSR321 = 0x01,
	// TCCR0 
	FOC0   = 0x80,
	WGM00  = 0x40,
	COM01  = 0x20,
	COM00  = 0x10,
	WGM01  = 0x08,
	CS02   = 0x04,
	CS01   = 0x02,
	CS00   = 0x01,
	// ASSR
	AS0    = 0x08,
	TCN0UB = 0x04,
	OCR0UB = 0x02,
	TCR0UB = 0x01,
	// TCCR1A, TCCR3A
	COMnA1 = 0x80,
	COMnA0 = 0x40,
	COMnB1 = 0x20,
	COMnB0 = 0x10,
	COMnC1 = 0x08,
	COMnC0 = 0x04,
	WGMn1  = 0x02,
	WGMn0  = 0x01,
	// TCCR1B, TCCR3B
	ICnC1  = 0x80,
	ICESn  = 0x40,
	WGMn3  = 0x10,
	WGMn2  = 0x08,
	CSn2   = 0x04,
	CSn1   = 0x02,
	CSn0   = 0x01,
	// TCCR1C, TCCR3C
	FOCnA  = 0x80,
	FOCnB  = 0x40,
	FOCnC  = 0x10,
	// TCCR2
	FOC2   = 0x80,
	WGM20  = 0x40,
	COM21  = 0x20,
	COM20  = 0x10,
	WGM21  = 0x08,
	CS22   = 0x04,
	CS21   = 0x02,
	CS20   = 0x01,
    };

    // Register Settings
    enum {
    	TIMER_PRESCALE_1    = CSn0,
	TIMER_PRESCALE_8    = CSn1,
	TIMER_PRESCALE_64   = CSn1 | CSn0,
	TIMER_PRESCALE_256  = CSn2,
	TIMER_PRESCALE_1024 = CSn2 | CSn0
    };


public:

    

protected:

    typedef AVR8::Reg8 Reg8;

    static Reg8 timsk() { return AVR8::in8(IO::TIMSK); }
    static void timsk(Reg8 value) { AVR8::out8(IO::TIMSK,value); }
    static Reg8 etimsk() { return AVR8::in8(IO::ETIMSK); }
    static void etimsk(Reg8 value) { AVR8::out8(IO::ETIMSK,value); }
    static Reg8 tifr() { return AVR8::in8(IO::TIFR); }
    static void tifr(Reg8 value) { AVR8::out8(IO::TIFR,value); }
    static Reg8 etifr() { return AVR8::in8(IO::ETIFR); }
    static void etifr(Reg8 value) { AVR8::out8(IO::ETIFR,value); }
    static Reg8 sfior() { return AVR8::in8(IO::SFIOR); }
    static void sfior(Reg8 value) { AVR8::out8(IO::SFIOR,value); }
    static Reg8 tccr0() { return AVR8::in8(IO::TCCR0); }
    static void tccr0(Reg8 value) { AVR8::out8(IO::TCCR0,value); }
    static Reg8 tcnt0() { return AVR8::in8(IO::TCNT0); }
    static void tcnt0(Reg8 value) { AVR8::out8(IO::TCNT0,value); }
    static Reg8 ocr0() { return AVR8::in8(IO::OCR0); }
    static void ocr0(Reg8 value) { AVR8::out8(IO::OCR0,value); }
    static Reg8 assr() { return AVR8::in8(IO::ASSR); }
    static void assr(Reg8 value) { AVR8::out8(IO::ASSR,value); }
    static Reg8 tccr1a() { return AVR8::in8(IO::TCCR1A); }
    static void tccr1a(Reg8 value) { AVR8::out8(IO::TCCR1A,value); }
    static Reg8 tccr1b() { return AVR8::in8(IO::TCCR1B); }
    static void tccr1b(Reg8 value) { AVR8::out8(IO::TCCR1B,value); }
    static Reg8 tccr1c() { return AVR8::in8(IO::TCCR1C); }
    static void tccr1c(Reg8 value) { AVR8::out8(IO::TCCR1C,value); }
    static Reg8 tcnt1h() { return AVR8::in8(IO::TCNT1H); }
    static void tcnt1h(Reg8 value) { AVR8::out8(IO::TCNT1H,value); }
    static Reg8 tcnt1l() { return AVR8::in8(IO::TCNT1L); }
    static void tcnt1l(Reg8 value) { AVR8::out8(IO::TCNT1L,value); }
    static Reg8 ocr1ah() { return AVR8::in8(IO::OCR1AH); }
    static void ocr1ah(Reg8 value) { AVR8::out8(IO::OCR1AH,value); }
    static Reg8 ocr1al() { return AVR8::in8(IO::OCR1AL); }
    static void ocr1al(Reg8 value) { AVR8::out8(IO::OCR1AL,value); }
    static Reg8 ocr1bh() { return AVR8::in8(IO::OCR1BH); }
    static void ocr1bh(Reg8 value) { AVR8::out8(IO::OCR1BH,value); }
    static Reg8 ocr1bl() { return AVR8::in8(IO::OCR1BL); }
    static void ocr1bl(Reg8 value) { AVR8::out8(IO::OCR1BL,value); }
    static Reg8 ocr1ch() { return AVR8::in8(IO::OCR1CH); }
    static void ocr1ch(Reg8 value) { AVR8::out8(IO::OCR1CH,value); }
    static Reg8 ocr1cl() { return AVR8::in8(IO::OCR1CL); }
    static void ocr1cl(Reg8 value) { AVR8::out8(IO::OCR1CL,value); }
    static Reg8 icr1h() { return AVR8::in8(IO::ICR1H); }
    static void icr1h(Reg8 value) { AVR8::out8(IO::ICR1H,value); }
    static Reg8 icr1l() { return AVR8::in8(IO::ICR1L); }
    static void icr1l(Reg8 value) { AVR8::out8(IO::ICR1L,value); }
    static Reg8 tccr2() { return AVR8::in8(IO::TCCR2); }
    static void tccr2(Reg8 value) { AVR8::out8(IO::TCCR2,value); }
    static Reg8 tcnt2() { return AVR8::in8(IO::TCNT2); }
    static void tcnt2(Reg8 value) { AVR8::out8(IO::TCNT2,value); }
    static Reg8 ocr2() { return AVR8::in8(IO::OCR2); }
    static void ocr2(Reg8 value) { AVR8::out8(IO::OCR2,value); }
    static Reg8 tccr3a() { return AVR8::in8(IO::TCCR3A); }
    static void tccr3a(Reg8 value) { AVR8::out8(IO::TCCR3A,value); }
    static Reg8 tccr3b() { return AVR8::in8(IO::TCCR3B); }
    static void tccr3b(Reg8 value) { AVR8::out8(IO::TCCR3B,value); }
    static Reg8 tccr3c() { return AVR8::in8(IO::TCCR3C); }
    static void tccr3c(Reg8 value) { AVR8::out8(IO::TCCR3C,value); }
    static Reg8 tcnt3h() { return AVR8::in8(IO::TCNT3H); }
    static void tcnt3h(Reg8 value) { AVR8::out8(IO::TCNT3H,value); }
    static Reg8 tcnt3l() { return AVR8::in8(IO::TCNT3L); }
    static void tcnt3l(Reg8 value) { AVR8::out8(IO::TCNT3L,value); }
    static Reg8 ocr3ah() { return AVR8::in8(IO::OCR3AH); }
    static void ocr3ah(Reg8 value) { AVR8::out8(IO::OCR3AH,value); }
    static Reg8 ocr3al() { return AVR8::in8(IO::OCR3AL); }
    static void ocr3al(Reg8 value) { AVR8::out8(IO::OCR3AL,value); }
    static Reg8 ocr3bh() { return AVR8::in8(IO::OCR3BH); }
    static void ocr3bh(Reg8 value) { AVR8::out8(IO::OCR3BH,value); }
    static Reg8 ocr3bl() { return AVR8::in8(IO::OCR3BL); }
    static void ocr3bl(Reg8 value) { AVR8::out8(IO::OCR3BL,value); }
    static Reg8 ocr3ch() { return AVR8::in8(IO::OCR3CH); }
    static void ocr3ch(Reg8 value) { AVR8::out8(IO::OCR3CH,value); }
    static Reg8 ocr3cl() { return AVR8::in8(IO::OCR3CL); }
    static void ocr3cl(Reg8 value) { AVR8::out8(IO::OCR3CL,value); }
    static Reg8 icr3h() { return AVR8::in8(IO::ICR3H); }
    static void icr3h(Reg8 value) { AVR8::out8(IO::ICR3H,value); }
    static Reg8 icr3l() { return AVR8::in8(IO::ICR3L); }
    static void icr3l(Reg8 value) { AVR8::out8(IO::ICR3L,value); }



};



class ATmega16_Timer0: public AVR_Timer
{

public:

    static const unsigned int CLOCK = Traits<CPU>::CLOCK >> 10;

    void frequency(const unsigned short & f){
    	unsigned short count = freq2cnt(f);
	Timer_Common::frequency(cnt2freq(count));
	ocr0(count);
	tccr0(WGM01 | TIMER_PRESCALE_1024);
    };

    void reset() {  }    

    void enable(){ timsk(timsk() | OCIE0); }
    void disable(){ timsk(timsk() & ~OCIE0); }

protected:

    static Hertz cnt2freq(unsigned short c) { return CLOCK / c; }
    static unsigned short freq2cnt(const Hertz & f) { return CLOCK / f; }

};


class ATmega128_Timer0: public ATmega16_Timer0
{

protected:

    // Register Settings
    enum {
    	TIMER_PRESCALE_1    = CSn0,
	TIMER_PRESCALE_8    = CSn1,
	TIMER_PRESCALE_32   = CSn1 | CSn0,
	TIMER_PRESCALE_64   = CSn2,
	TIMER_PRESCALE_128  = CSn2 | CSn0,
	TIMER_PRESCALE_256  = CSn2 | CSn1,
	TIMER_PRESCALE_1024 = CSn2 | CSn1 | CSn0
    };

public:

    void frequency(const unsigned short & f){
    	unsigned short count = freq2cnt(f);
	Timer_Common::frequency(cnt2freq(count));
	ocr0(count);
	tccr0(WGM01 | TIMER_PRESCALE_1024);
    };

};



class AVR_Timer1: public AVR_Timer
{

protected:


public:


};

class AVR_Timer2: public AVR_Timer
{


protected:


public:






};


class AVR_Timer3: public AVR_Timer
{

public:


protected:



};

__END_SYS

#endif
