// EPOS Cortex-A53 BCM Timer Mediator Declarations

#ifndef __cortex_a53_bcm_timer_h
#define __cortex_a53_bcm_timer_h

#include <architecture/cpu.h>
#define __common_only__
#include <machine/rtc.h>
#include <machine/timer.h>
#undef __common_only__

__BEGIN_SYS

class BCM_Timer : public Timer_Common
{

    // This is a hardware object.
    // Use with something like "new (Memory_Map::TIMER0_BASE) BCM_Timer".

private:
    typedef CPU::Reg32 Reg32;
    typedef TSC_Common::Hertz Hertz;

public:
    typedef CPU::Reg64 Count;

    static const unsigned int CLOCK = 1000000000;

    // BCM SYSTEM TIMER offsets from base
    enum {                                      // Description
        STCS                        = 0x00,     // Control/Status
        STCLO                       = 0x04,     // Low COUNTER
        STCHI                       = 0x08,     // High Counter
        STC0                        = 0x0c,     // Compare 0 - Used by GPU
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC2                        = 0X14,     // Compare 2 - Used by GPU
        STC3                        = 0X18      // Compare 3 - Value used to generate interrupt 3
        // Interrupts mapped to "Enable IRQ 1" - c1 and c3 == irq1 and irq3
    };

public:
    void config(unsigned int unit = 1, const Count & count) {
        if (unit == 1) {
            bcmt(STCS) |= 1 << 1;
            bcmt(STC1) = count + bcmt(STCLO);
        } else if (unit == 3) {
            bcmt(STCS) |= 1 << 3;
            bcmt(STC3) = count + bcmt(STCLO);
        }
    }

    Count count() {
        //Reg32 high;
        Reg32 low;

        //do {
        //    high = bcmt(STCHI);
        low = bcmt(STCLO);
        //} while(bcm(STCHI) != high);

        return low;

        //return static_cast<Count>(high) << 32 | low;
    }

    Hertz clock() { return CLOCK; }

protected:
    volatile Reg32 & bcmt(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
};

};

__END_SYS

#endif