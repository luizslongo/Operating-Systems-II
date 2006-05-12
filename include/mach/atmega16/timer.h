// EPOS-- ATMega16 Timer Mediator Declarations

#ifndef __atmega16_timer_h
#define __atmega16_timer_h

#include "../avr_common/timer.h"

__BEGIN_SYS

class ATMega16_Timer: public Timer_Common, private AVR_Timer
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK >> 10;

public:
    ATMega16_Timer() {}

    ATMega16_Timer(const Hertz & f) {
	db<PC_Timer>(TRC) << "ATMega16_Timer(f=" << f << ")\n";
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

__END_SYS

#endif
