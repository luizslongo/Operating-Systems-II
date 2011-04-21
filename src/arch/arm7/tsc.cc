// EPOS-- ARM7_TSC Implementation

#include <arch/arm7/tsc.h>
//TODO: how to avoid breaking the Arch <=> Mach isolation?
#include <timer.h>


__BEGIN_SYS

volatile unsigned long ARM7_TSC::_ts = 0;
TSC_Timer ARM7_TSC::_timer(ARM7_TSC::RESOLUTION,
        ARM7_TSC::tsc_timer_handler);


ARM7_TSC::Time_Stamp ARM7_TSC::time_stamp() {
    return _timer.read() | _ts << (sizeof(CPU::Reg16) * 8);
}

void ARM7_TSC::tsc_timer_handler() {
    _ts++;
}

__END_SYS
