#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool debugged = true;
};


// Utilities
template <> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = false;
    static const bool trace   = false;
};

template <> struct Traits<Heap>: public Traits<void>
{
};


// System parts
template <> struct Traits<Boot>: public Traits<void>
{
};

template <> struct Traits<Setup>: public Traits<void>
{
};

template <> struct Traits<Init>: public Traits<void>
{
};

template <> struct Traits<System>: public Traits<void>
{
};


// Mediators
template <> struct Traits<IA32>: public Traits<void>
{
//    static const int CLOCK = 2003402000;
    static const int CLOCK = 797886000;
};

template <> struct Traits<AVR8>: public Traits<void>
{
  //    static const unsigned long long CLOCK = 7372800; //Mica2
    static const unsigned long long CLOCK = 8000000; //ATMega128 / ATMega16
};

template <> struct Traits<IA32_MMU>: public Traits<void>
{
    static const bool fast_log_to_phy = false;
};

template <> struct Traits<AVR8_MMU>: public Traits<void>
{
    static const bool fast_log_to_phy = false;
};

template <> struct Traits<PC_RTC>: public Traits<void>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega16_RTC>: public Traits<void>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega128_RTC>: public Traits<void>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<PC>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x00008000;
    static const unsigned int HARDWARE_INT_OFFSET = 0x20;
    static const unsigned int SYSCALL_INT = 0x80;

    static const unsigned int SYSTEM_STACK_SIZE = 4096;
    static const unsigned int SYSTEM_HEAP_SIZE = 4096;
    static const unsigned int APPLICATION_STACK_SIZE = 4096;
    static const unsigned int APPLICATION_HEAP_SIZE = 40960;
};

template <> struct Traits<ATMega16>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;
    static const int INT_BASE = 0x00; 
    static const int INT_VEC_SIZE = 24; 

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 64;
    static const unsigned int APPLICATION_STACK_SIZE = 64;
    static const unsigned int APPLICATION_HEAP_SIZE = 128;
};

template <> struct Traits<ATMega128>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;
    static const int INT_BASE = 0x00; 
    static const int INT_VEC_SIZE = 35; 

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 64;
    static const unsigned int APPLICATION_STACK_SIZE = 64;
    static const unsigned int APPLICATION_HEAP_SIZE = 512;
};

template <> struct Traits<PC_PCI>: public Traits<void>
{
    static const int MAX_BUS = 0;
    static const int MAX_DEV_FN = 0xff;
};

template <> struct Traits<PC_Timer>: public Traits<void>
{
    // Meaningful values for the PC's timer frequency range from 100 to 
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<ATMega16_Timer>: public Traits<void>
{
    static const int FREQUENCY = 40; // Hz
};

template <> struct Traits<ATMega128_Timer>: public Traits<void>
{
    // Should be between 28 and 7200 Hz
    static const int FREQUENCY = 720; // Hz
    
};

template <> struct Traits<PC_UART>: public Traits<void>
{
    static const unsigned int CLOCK = 1843200; // 1.8432 MHz
    static const unsigned int COM1 = 0x3f8; // to 0x3ff, IRQ4
    static const unsigned int COM2 = 0x2f8; // to 0x2ff, IRQ3
    static const unsigned int COM3 = 0x3e8; // to 0x3ef, no IRQ
    static const unsigned int COM4 = 0x2e8; // to 0x2ef, no IRQ
};

template <> struct Traits<PC_NIC>: public Traits<void>
{
    static const unsigned int PCNET32_UNITS = 1;
    static const unsigned int PCNET32_SEND_BUFFERS = 8; // per unit
    static const unsigned int PCNET32_RECEIVE_BUFFERS = 8; // per unit
};

template <> struct Traits<PC_Display>: public Traits<void>
{
    static const bool on_serial = false;
    static const int COLUMNS = 80;
    static const int LINES = 25;
    static const int TAB_SIZE = 8;
    static const unsigned int FRAME_BUFFER_ADDRESS = 0xb8000;
};

template <> struct Traits<ATMega128_Display>: public Traits<void>
{
    static const bool on_serial = false;
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};


// Abstractions
template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
};

template <> struct Traits<Thread>: public Traits<void>
{
    static const bool idle_waiting = true;
    static const bool active_scheduler = true;
    static const unsigned int quantum = 10000; // us
    static const bool preemptive = true;
    static const bool smp = false;
};

template <> struct Traits<Synchronizer>: public Traits<void>
{
    static const bool strict_ordering = true;
};

template <> struct Traits<Network>: public Traits<void>
{
    static const unsigned int EPOS_LIGHT_PROTOCOL = 0x8888;
};

__END_SYS

#endif
