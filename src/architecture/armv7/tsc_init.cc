// EPOS ARMv7 Time-Stamp Counter Mediator Initialization


#if defined(__cortex_a__)

#include <machine/timer.h>

__BEGIN_SYS

void TSC::init()
{
    db<Init, TSC>(TRC) << "TSC::init()" << endl;


    if(CPU::id() == 0) {
        // Disable counting before programming
        reg(GTCLR) = 0;

        // Set timer to 0
        reg(GTCTRL) = 0;
        reg(GTCTRH) = 0;

        // Re-enable counting
        reg(GTCLR) = 1;
    }
}

__END_SYS

#elif defined(__cortex_m__)

#include <machine/timer.h>
#include "sysctrl.h"
#include "ioctrl.h"
#include "memory_map.h"

__BEGIN_SYS

void TSC::init()
{
    db<Init, TSC>(TRC) << "TSC::init()" << endl;

    SysCtrl * sc = reinterpret_cast<SysCtrl *>(Memory_Map::SCR_BASE);
    GPTM * gptm = reinterpret_cast<GPTM *>(Memory_Map::TIMER_BASE + 0x1000 * unit);

    sc->clock_timer(Traits<Timer>::UNITS - 1);
    gptm->config(0xffffffff, true, (Traits<Build>::MODEL == Traits<Build>::LM3S811) ? false : true);

    // time-out interrupt will be registered later at IC::init(), because IC hasn't been initialized yet
}

__END_SYS

#endif
