// EPOS-- ATMega128 Timer Mediator Declarations

#ifndef __atmega128_timer_h
#define __atmega128_timer_h

#include "../avr_common/timer.h"

__BEGIN_SYS

class ATMega128_Timer: public Timer_Common, private AVR_Timer
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK >> 10;

public:
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
    ATMega128_Timer() {}

    ATMega128_Timer(const Hertz & f) {
	db<PC_Timer>(TRC) << "ATMega128_Timer(f=" << f << ")\n";
	frequency(f);
    }

    Hertz frequency() const { return count2freq(ocr0()); }
    void frequency(const Hertz & f) {
	ocr0(freq2count(f));
	tccr0(WGM01 | TIMER_PRESCALE_1024);
    };

    void reset() { tcnt0(0); }

    void enable(){ timsk(timsk() | OCIE0); }
    void disable(){ timsk(timsk() & ~OCIE0); }

    Tick read() { return tcnt0(); }

protected:
    static Hertz count2freq(const Count & c) { return CLOCK / c; }
    static Count freq2count(const Hertz & f) { return CLOCK / f; }
};

class ATMega128_Timer_2: public Timer_Common, private AVR_Timer
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK >> 9;

public:
    // Register Settings
    enum {
    	TIMER_PRESCALE_1    = CSn0,
	TIMER_PRESCALE_8    = CSn1,
	TIMER_PRESCALE_64   = CSn1 | CSn0,
	TIMER_PRESCALE_256  = CSn2,
	TIMER_PRESCALE_1024 = CSn2 | CSn0
    };

public:
    ATMega128_Timer_2() {}

    ATMega128_Timer_2(const Hertz & f) {
	frequency(f);
    }

    Hertz frequency() const { return count2freq(ocr2()); }
    void frequency(const Hertz & f) {
	ocr2(freq2count(f));
	tccr2(WGM21 | TIMER_PRESCALE_1024);
    };

    void reset() { tcnt2(0); }

    void enable(){ timsk(timsk() | OCIE2); }
    void disable(){ timsk(timsk() & ~OCIE2); }

    Tick read() { return tcnt2(); }

protected:
    static Hertz count2freq(const Count & c) { return CLOCK / c; }
    static Count freq2count(const Hertz & f) { return CLOCK / f; }
};



__END_SYS

#endif
