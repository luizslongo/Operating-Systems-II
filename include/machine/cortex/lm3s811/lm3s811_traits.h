// EPOS LM3S811 (ARM Cortex-M3) Metainfo and Configuration

#ifndef __lm3s811_traits_h
#define __lm3s811_traits_h

#include <system/config.h>

__BEGIN_SYS

class Machine_Common;
template<> struct Traits<Machine_Common>: public Traits<Build>
{
    static const bool debugged = Traits<Build>::debugged;
};

template<> struct Traits<Machine>: public Traits<Machine_Common>
{
    static const bool cpus_use_local_timer      = false;

    static const unsigned int NOT_USED          = 0xffffffff;
    static const unsigned int CPUS              = Traits<Build>::CPUS;

    // Physical Memory
    static const unsigned int MEM_BASE          = 0x20000000;
    static const unsigned int MEM_TOP           = 0x20001fff;   // 8 KB (MAX for 32-bit is 0x70000000 / 1792 MB)
    static const unsigned int MIO_BASE          = 0x40000000;
    static const unsigned int MIO_TOP           = 0x440067ff;

    // Boot Image
    static const unsigned int BOOT_LENGTH_MIN   = NOT_USED;
    static const unsigned int BOOT_LENGTH_MAX   = NOT_USED;
    static const unsigned int BOOT_STACK        = 0x20001ffc;   // MEM_TOP - sizeof(int)

    // Logical Memory Map
    static const unsigned int BOOT              = NOT_USED;
    static const unsigned int IMAGE             = NOT_USED;
    static const unsigned int SETUP             = NOT_USED;
    static const unsigned int INIT              = NOT_USED;

    static const unsigned int APP_LOW           = MEM_BASE;
    static const unsigned int APP_CODE          = 0x00000000;   // Flash
    static const unsigned int APP_DATA          = APP_LOW;
    static const unsigned int APP_HIGH          = MEM_TOP;
    static const unsigned int VECTOR_TABLE      = 0x00200000;

    static const unsigned int PHY_MEM           = NOT_USED;     // this machine only supports the library architecture of EPOS
    static const unsigned int IO                = NOT_USED;     // this machine only supports the library architecture of EPOS
    static const unsigned int SYS               = NOT_USED;     // this machine only supports the library architecture of EPOS

    // Default Sizes and Quantities
    static const unsigned int STACK_SIZE        = 512;
    static const unsigned int HEAP_SIZE         = 512;
    static const unsigned int MAX_THREADS       = 5;
};

template<> struct Traits<IC>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    static const unsigned int IRQS = 48;
    static const unsigned int INTS = 65;
};

template<> struct Traits<Timer>: public Traits<Machine_Common>
{
    static const bool debugged = hysterically_debugged;

    static const unsigned int UNITS = 4; // GPTM

    // Meaningful values for the timer frequency range from 100 to
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template<> struct Traits<UART>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 2;

    static const unsigned int CLOCK = Traits<CPU>::CLOCK;

    static const unsigned int DEF_UNIT = 0;
    static const unsigned int DEF_BAUD_RATE = 115200;
    static const unsigned int DEF_DATA_BITS = 8;
    static const unsigned int DEF_PARITY = 0; // none
    static const unsigned int DEF_STOP_BITS = 1;
};

template<> struct Traits<GPIO>: public Traits<Machine_Common>
{
    static const unsigned int UNITS = 5;
    static const bool supports_power_up = false;
};

template<> struct Traits<Serial_Display>: public Traits<Machine_Common>
{
    static const bool enabled = (Traits<Build>::EXPECTED_SIMULATION_TIME != 0);
    static const int ENGINE = UART;
    static const int UNIT = 0;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

template<> struct Traits<Serial_Keyboard>: public Traits<Machine_Common>
{
    static const bool enabled = (Traits<Build>::EXPECTED_SIMULATION_TIME != 0);
};

template<> struct Traits<Scratchpad>: public Traits<Machine_Common>
{
    static const bool enabled = false;
};

__END_SYS

#endif
