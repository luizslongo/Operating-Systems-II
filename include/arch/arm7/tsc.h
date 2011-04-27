// EPOS-- ARM7 Time-Stamp Counter Mediator Declarations

#ifndef __arm7_tsc_h
#define __arm7_tsc_h

#include <tsc.h>
#include <cpu.h>

__BEGIN_SYS

class ARM7_TSC: public TSC_Common
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK;
    static const unsigned int RESOLUTION = (2^sizeof(CPU::Reg16))/CLOCK;

public:
    ARM7_TSC() {}

    static Hertz frequency() {return CLOCK;}

    static void init() {}

    static Time_Stamp time_stamp();

private:
    static void tsc_timer_handler();

    static volatile unsigned long _ts;
    static TSC_Timer _timer;
};

__END_SYS

#endif
