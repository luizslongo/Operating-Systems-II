#ifndef __mc13224v_traits_h
#define __mc13224v_traits_h

#include <system/config.h>
#include <system/types.h>

// Mediators - Machine - MC13224V

__BEGIN_SYS

class MC13224V_Common;

template <> struct Traits<MC13224V_Common>: public Traits<void>
{
};

template <> struct Traits<MC13224V>: public Traits<MC13224V_Common>
{
	static const unsigned int MAX_CPUS = 1;
	static const unsigned int CLOCK = 24000000;
	static const unsigned int APPLICATION_STACK_SIZE = 1024;
	static const unsigned int APPLICATION_HEAP_SIZE = 3072;
	static const unsigned int SYSTEM_HEAP_SIZE = 2*APPLICATION_HEAP_SIZE;
	static const bool SMP = false;
};

template <> struct Traits<MC13224V_Battery>: public Traits<MC13224V_Common>
{
    static const bool enabled = true;
    static const bool buck_enabled = true;
    static const unsigned int buck_voltage_threshold = 2500; // in mV
    static const unsigned int frequency = 1; // in Hz
};

template <> struct Traits<MC13224V_Timer_0>: public Traits<MC13224V_Common>
{
    static const int FREQUENCY = 1000; // default system timer, in Hz
};

template <> struct Traits<MC13224V_Timer_1>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 100;
};

template <> struct Traits<MC13224V_Timer_2>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 100;
};

template <> struct Traits<MC13224V_Timer_3>: public Traits<MC13224V_Common>
{
	static const int FREQUENCY = 100;
};

template <> struct Traits<MC13224V_RTC>: public Traits<MC13224V_Common>
{
	static const unsigned int EPOCH_DAYS = 1;
};

template <> struct Traits<MC13224V_Display>: public Traits<MC13224V_Common>
{
	static const bool on_serial = true;
};

template <> struct Traits<MC13224V_NIC>: public Traits<void>
{
    static const bool enabled = true;

    typedef LIST<MC13224V_Radio> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<MC13224V_Radio>::Result;
};

template <> struct Traits<MC13224V_ADC>: public Traits<void>
{
    enum Power_Modes
    {
        FULL        = 0,
        LIGHT       = 1,
        STANDBY     = 2,
        OFF         = 3
    };
};

__END_SYS

#endif

