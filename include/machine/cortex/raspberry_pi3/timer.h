// EPOS Realview PBX (ARM Cortex-A9) Timer Mediator Declarations

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
    typedef TSC_Common::Hertz Hertz;
    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef A9_Private_Timer::Count Count;
    static const unsigned int RELOAD_VALUE = 1000;
    static const unsigned int UNIT = 1;

public:
    System_Timer_Engine() { new(bcmt()) BCM_Timer; }

    Count read() { return bcmt()->read(); }

    void enable() { }
    void disable() { }

    Hertz clock() { return bcmt()->clock(); }

protected:
    static void eoi(const Interrupt_Id & id) {
        bcmt()->config(UNIT, RELOAD_VALUE);
    }

private:
    static void init(const Hertz & frequency) {
        bcmt()->config(UNIT, RELOAD_VALUE);
    }

private:
    static BCM_Timer * bcmt() { return reinterpret_cast<BCM_Timer *>(Memory_Map::TIMER0_BASE); }
};

// TODO: Discuss this, as BCM_TIMER has only one "user timer" wich is the "Compare 3" (STC3)
class User_Timer_Engine: public Timer_Common
{
private:
    static const unsigned int UNITS = 3;//Traits<Timer>::UNITS;

    typedef IC_Common::Interrupt_Id Interrupt_Id;

public:
    typedef BCM_Timer::Count Count;

public:
    // TODO: only one unit is possible, shall we change the function parameters?
    User_Timer_Engine(unsigned int unit, const Microsecond & time, bool interrupt, bool periodic) : _periodic(periodic), _time(time) {
        assert(unit == UNITS);
        new (bcmt()) BCM_Timer;
        bcmt()->config(UNITS, time);
    }

    ~User_Timer_Engine() { bcmt()->config(UNITS, 0); }

    Count count() const { return bmct()->count(); }

    void enable() const  { }
    void disable() const { }

    Hertz clock() const { return bmct()->clock(); }

protected:
    static void eoi(const Interrupt_Id & id) {
        if (periodic)
            bcmt()->config(UNITS, time);
        else
            bcmt()->config(UNITS, 0);
    };

private:
    bool _periodic;
    Microsecond time;
    static BCM_Timer * bcmt() { return reinterpret_cast<BCM_Timer *>(Memory_Map::TIMER0_BASE); }
};

__END_SYS

#endif
