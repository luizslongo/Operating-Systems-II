// EPOS LM3S811 (ARM Cortex-M3) Mediator Declarations

#ifndef __lm3s811_h
#define __lm3s811_h

#define __common_only__
#include <machine/machine.h>
#include <machine/rtc.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_m3/scb.h>
#include <machine/cortex/engines/cortex_m3/systick.h>
#include "sysctrl.h"
#include "memory_map.h"
#include <system.h>

__BEGIN_SYS

class LM3S811: public Machine_Common
{
private:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;
    typedef RTC_Common::Microsecond Microsecond;

public:
    LM3S811() {}

    static void delay(const Microsecond & time) { systick()->delay(time); }

    static void reboot() { scb()->reboot(); }
    static void poweroff() { reboot(); }

    static const UUID & uuid() { return System::info()->bm.uuid; } // TODO: System_Info is not populated in this machine

    static void smp_barrier() {}

    static void smp_init(unsigned int n_cpus) { assert(n_cpus == 1); }

protected:
    static void pre_init();
    static void init();

private:
    static SysTick * systick() { return reinterpret_cast<SysTick *>(SCS_BASE); }
    static SCB * scb() { return reinterpret_cast<SCB *>(SCS_BASE); }
    static SysCtrl * scr() { return reinterpret_cast<SysCtrl *>(SCR_BASE); }
};

typedef LM3S811 Machine_Model;

__END_SYS

#endif
