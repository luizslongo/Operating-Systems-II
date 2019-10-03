// EPOS Raspberry Pi3 (ARM Cortex-A53) Timer Mediator Declarations

#ifndef __raspberry_pi3_timer_h
#define __raspberry_pi3_timer_h

#define __common_only__
#include <architecture/tsc.h>
#include <machine/ic.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_a53/bcm_timer.h>
#include "memory_map.h"
#include <utility/convert.h>

__BEGIN_SYS

class System_Timer_Engine: public Timer_Common
{
    friend Timer; // for init()

private:
    static const unsigned int UNIT = 0;

    typedef TSC_Common::Hertz Hertz;
    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef BCM_Timer::Count Count;

public:
    Count read() { return timer()->read(); }

    void enable() { timer()->enable(); }
    void disable() { timer()->disable(); }

    Hertz clock() { return timer()->clock(); }

protected:
    static void eoi(const Interrupt_Id & id) { timer()->config(UNIT, _count); };

private:
    static void init(const Hertz & frequency) {
        timer()->config(0, timer()->clock() / frequency);
        timer()->enable();
    }

private:
    static BCM_Timer * timer() { return reinterpret_cast<BCM_Timer *>(Memory_Map::TIMER0_BASE); }
};


class User_Timer_Engine: public Timer_Common
{
    friend Timer; // for init()

private:
    static const unsigned int UNITS = Traits<Timer>::UNITS;
    static const unsigned int UNIT = 1;

    typedef TSC_Common::Hertz Hertz;
    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef BCM_Timer::Count Count;

public:
    User_Timer_Engine(unsigned int unit, const Microsecond & time, bool interrupt, bool periodic): _count(Convert::us2count(time, clock())) {
        assert(unit < UNITS);
        power(FULL);
        timer()->config(UNIT, _count);
    }
    ~User_Timer_Engine() {
        power(OFF);
    }

    Count read() { return timer()->read(); }

    void enable() { timer()->enable(); }
    void disable() { timer()->disable(); }

    Hertz clock() { return timer()->clock(); }

    void power(const Power_Mode & mode) {
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL:
        case LIGHT:
        case SLEEP:
            // TODO: clock_timer()
            break;
        case OFF:
            break;
        }
    }

protected:
    static void eoi(const Interrupt_Id & id) { timer()->config(UNIT, _count); };

private:
    static void init(const Hertz & frequency) {
        timer()->config(1, timer()->clock() / frequency);
        timer()->enable();
    }

private:
    static BCM_Timer * timer() { return reinterpret_cast<BCM_Timer *>(Memory_Map::TIMER0_BASE); }

private:
    Count _count;
};

__END_SYS

#endif
