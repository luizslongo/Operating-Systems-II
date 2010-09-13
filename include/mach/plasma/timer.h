// EPOS PLASMA Timer Mediator Declarations

#ifndef __plasma_timer_h
#define __plasma_timer_h

#include <cpu.h>
#include <ic.h>
#include <timer.h>

__BEGIN_SYS

class PLASMA_Timer:  public Timer_Common
{
private:
    typedef CPU::Reg32 Count;

    // Plasma Timer private imports, types and constants
    static const unsigned int CLOCK = Traits<PLASMA>::CLOCK;

public:
    // enuns
 
public:
    PLASMA_Timer(int unit = 0) {} // actual initialization is up to init

    ~PLASMA_Timer() {}

    void frequency(const Hertz & f) {
        _count = freq2cnt(f);
        reset();
        db<PLASMA_Timer>(TRC) << "PLASMA_Timer::frequency(freq=" << frequency()
		                             << ",cnt=" << (void*)_count << ")\n";
    }

    Hertz frequency() {  return cnt2freq(_count); }

    void enable() {}

    void disable() {}

    void reset() {}

    static void int_handler(unsigned int interrupt); //will be overriden by alarm

    static void init();

private:
    static Hertz cnt2freq(unsigned int c) { return CLOCK / c; }
    static unsigned int freq2cnt(const Hertz & f) { return CLOCK / f; }
	
private:
    // Plasma_Timer attributes
    static Count _count;

};

__END_SYS

#endif
