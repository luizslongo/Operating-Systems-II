// EPOS-- ARM7 Time-Stamp Counter Mediator Declarations

#ifndef __arm7_tsc_h
#define __arm7_tsc_h

#include <tsc.h>

__BEGIN_SYS

class ARM7_TSC: public TSC_Common
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK;

public:
    ARM7_TSC() {}

    static Hertz frequency() { return CLOCK; }

    static Time_Stamp time_stamp() {
    Time_Stamp ts;
    //ASMV("rdtsc" : "=A" (ts) : ); // must be volatile!
    return ts;
    }

};

__END_SYS

#endif
