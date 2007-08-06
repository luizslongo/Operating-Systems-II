// EPOS-- MIPS_TSC Declarations

#ifndef __mips32_tsc_h
#define __mips32_tsc_h

#include <cpu.h>
#include <tsc.h>

__BEGIN_SYS

class MIPS32_TSC: public TSC_Common
{
private:
    static const unsigned int CLOCK = Traits<Machine>::CLOCK;

public:
    MIPS32_TSC() {}

    static Hertz frequency() { return CLOCK; }

    static Time_Stamp time_stamp() {
	//Unimplemented ... Plasma doesn't have an TSC implemented (MIPS have?)
        return 0;
    }
};

__END_SYS

#endif
