// EPOS-- IA32 TSC Mediator Declarations

#ifndef __ia32_tsc_h
#define __ia32_tsc_h

#include <tsc.h>

__BEGIN_SYS

class IA32_TSC: public TSC_Common
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK;

public:
    IA32_TSC() {}

    static Hertz frequency() { return CLOCK; }

    static Time_Stamp time_stamp() {
	Time_Stamp ts;
	ASMV("rdtsc" : "=A" (ts) : ); // must be volatile!
	return ts;
    }

    static int init(System_Info * si) { return 0; }
};

__END_SYS

#endif
