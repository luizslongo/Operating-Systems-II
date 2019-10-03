// EPOS ARM Cortex-A53 BCM Timer Mediator Declarations

#ifndef __bcm_timer_h
#define __bcm_timer_h

#include <architecture/cpu.h>
#define __common_only__
#include <machine/rtc.h>
#include <machine/timer.h>
#undef __common_only__

__BEGIN_SYS

class BCM_Timer : public Timer_Common
{
    // This is a hardware object
    // Use with something like "new (Memory_Map::TIMER0_BASE) BCM_Timer"

private:
    typedef CPU::Reg32 Reg32;
    typedef TSC_Common::Hertz Hertz;
    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef CPU::Reg64 Count;

    static const unsigned int CLOCK = 1000000000;

    // Registers offsets from BASE (i.e. this)
    enum {                                      // Description
        STCS    = 0x00,     // Control/Status
        STCLO   = 0x04,     // Low COUNTER
        STCHI   = 0x08,     // High Counter
        STC0    = 0x0c,     // Compare 0 - Used by GPU
        STC1    = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC2    = 0X14,     // Compare 2 - Used by GPU
        STC3    = 0X18      // Compare 3 - Value used to generate interrupt 3
        // Interrupts mapped to "Enable IRQ 1" - c1 and c3 == irq1 and irq3
    };

public:
    void config(unsigned int unit, const Count & count) {
        assert(unit < 2);
        if(unit == 0) {
            timer(STCS) |= 1 << 1;
            timer(STC1) = count + timer(STCLO);
        } else {
            timer(STCS) |= 1 << 3;
            timer(STC3) = count + timer(STCLO);
        }
    }

    Count count() {
        return static_cast<Count>(bcm(STCLO));

        // TODO: We could not identified why, but reading High counter does not work, it returns an execution error!
        //Reg32 high;
        //Reg32 low;
        //do {
        //    high = timer(STCHI);
        //    low = timer(STCLO);
        //} while(bcm(STCHI) != high);
        //return static_cast<Count>(high) << 32 | low;
    }

    Hertz clock() { return CLOCK; }

private:
    volatile Reg32 & timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

__END_SYS

#endif
