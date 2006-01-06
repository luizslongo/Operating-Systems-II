#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool debugged = false;
    static const bool initialize = true;
};


// Utilities
template <> struct Traits<Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = true;
    static const bool trace   = true;
};

template <> struct Traits<Heap>: public Traits<void>
{
};


// System parts
template <> struct Traits<Boot>: public Traits<void>
{
    static const bool debugged = true;
};

template <> struct Traits<Setup>: public Traits<void>
{
    static const bool debugged = true;
};

template <> struct Traits<Init>: public Traits<void>
{
    static const bool debugged = true;
};

template <> struct Traits<System>: public Traits<void>
{
    static const bool debugged = true;
};


// Mediators - Architecture - IA32
#ifdef __ia32
template <> struct Traits<IA32>: public Traits<void>
{
};

template <> struct Traits<IA32_TSC>: public Traits<void>
{
};

template <> struct Traits<IA32_MMU>: public Traits<void>
{
};
#endif

// Mediators - Architecture - AVR8
#ifdef __avr8
template <> struct Traits<AVR8>: public Traits<void>
{
};

template <> struct Traits<AVR8_TSC>: public Traits<void>
{
};

template <> struct Traits<AVR8_MMU>: public Traits<void>
{
};
#endif


// Mediators - Machine - Common
template <> struct Traits<Serial_Display>: public Traits<void>
{
    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 8;
};

// Mediators - Machine - PC
#ifdef __pc
template <> struct Traits<PC>: public Traits<void>
{
    static const unsigned int CLOCK = 797886000;

    static const unsigned int BOOT_IMAGE_ADDR = 0x00008000;
    static const unsigned int HARDWARE_INT_OFFSET = 0x20;
    static const unsigned int SYSCALL_INT = 0x80;

    static const unsigned int SYSTEM_STACK_SIZE = 4096;
    static const unsigned int SYSTEM_HEAP_SIZE = 4096;

    static const unsigned int APPLICATION_STACK_SIZE = 16 * 1024;
    static const unsigned int APPLICATION_HEAP_SIZE = 16 * 1024 * 1024;
};

template <> struct Traits<PC_PCI>: public Traits<void>
{
    static const int MAX_BUS = 0;
    static const int MAX_DEV_FN = 0xff;
};

template <> struct Traits<PC_IC>: public Traits<void>
{
};

template <> struct Traits<PC_Timer>: public Traits<void>
{
    // Meaningful values for the PC's timer frequency range from 100 to 
    // 10000 Hz. The choice must respect the scheduler time-slice, i. e.,
    // it must be higher than the scheduler invocation frequency.
    static const int FREQUENCY = 1000; // Hz
};

template <> struct Traits<PC_RTC>: public Traits<void>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<PC_EEPROM>: public Traits<void>
{
    static const bool initialize = false;
};

template <> struct Traits<PC_UART>: public Traits<void>
{
    static const unsigned int CLOCK = 1843200; // 1.8432 MHz
    static const unsigned int COM1 = 0x3f8; // to 0x3ff, IRQ4
    static const unsigned int COM2 = 0x2f8; // to 0x2ff, IRQ3
    static const unsigned int COM3 = 0x3e8; // to 0x3ef, no IRQ
    static const unsigned int COM4 = 0x2e8; // to 0x2ef, no IRQ
};

template <> struct Traits<PC_SPI>: public Traits<void>
{
    static const bool initialize = false;
};

template <> struct Traits<PC_Display>: public Traits<void>
{
    static const bool on_serial = true;
    static const int COLUMNS = 80;
    static const int LINES = 25;
    static const int TAB_SIZE = 8;
    static const unsigned int FRAME_BUFFER_ADDRESS = 0xb8000;
};

template <> struct Traits<PC_NIC>: public Traits<void>
{
    typedef LIST<PCNet32, PCNet32> NICS;

    static const unsigned int PCNET32_UNITS = NICS::Count<PCNet32>::Result;
    static const unsigned int PCNET32_SEND_BUFFERS = 8; // per unit
    static const unsigned int PCNET32_RECEIVE_BUFFERS = 8; // per unit

    static const unsigned int E100_UNITS = NICS::Count<E100>::Result;
    static const unsigned int E100_SEND_BUFFERS = 8; // per unit
    static const unsigned int E100_RECEIVE_BUFFERS = 8; // per unit

    static const unsigned int C905_UNITS = NICS::Count<C905>::Result;
    static const unsigned int C905_SEND_BUFFERS = 8; // per unit
    static const unsigned int C905_RECEIVE_BUFFERS = 8; // per unit
};
#endif

// Mediators - Machine - ATMega16
#ifdef __atmega16
class ATMega16_Common;
template <> struct Traits<ATMega16_Common>: public Traits<void>
{
    static const bool initialize = false;
};

template <> struct Traits<ATMega16>: public Traits<ATMega16_Common>
{
    static const unsigned long long CLOCK = 8000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 64;
    static const unsigned int APPLICATION_STACK_SIZE = 64;
    static const unsigned int APPLICATION_HEAP_SIZE = 256;
};

template <> struct Traits<ATMega16_Timer>: public Traits<ATMega16_Common>
{
    static const int FREQUENCY = 40; // Hz
};

template <> struct Traits<ATMega16_RTC>: public Traits<ATMega16_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega16_EEPROM>: public Traits<ATMega16_Common>
{
    static const unsigned int SIZE = 512; // bytes
};

template <> struct Traits<ATMega16_SPI>: public Traits<ATMega16_Common>
{
};

template <> struct Traits<ATMega16_Display>: public Traits<ATMega16_Common>
{
    static const bool on_serial = true;
};

template <> struct Traits<ATMega16_NIC>: public Traits<void>
{
    typedef LIST<Radio> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};
#endif


// Mediators - Machine - ATMega128
#ifdef __atmega128
class ATMega128_Common;
template <> struct Traits<ATMega128_Common>: public Traits<void>
{
    static const bool initialize = false;
};

template <> struct Traits<ATMega128>: public Traits<ATMega128_Common>
{
  //    static const unsigned long long CLOCK = 7372800; //Mica2
    static const unsigned long long CLOCK = 8000000;
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;

    static const unsigned int SYSTEM_STACK_SIZE = 64;
    static const unsigned int SYSTEM_HEAP_SIZE = 64;
    static const unsigned int APPLICATION_STACK_SIZE = 64;
    static const unsigned int APPLICATION_HEAP_SIZE = 512;
};

template <> struct Traits<ATMega128_Timer>: public Traits<ATMega128_Common>
{
    // Should be between 28 and 7200 Hz
    static const int FREQUENCY = 720; // Hz
    
};

template <> struct Traits<ATMega128_RTC>: public Traits<ATMega128_Common>
{
    static const unsigned int EPOCH_DAY = 1;
    static const unsigned int EPOCH_MONTH = 1;
    static const unsigned int EPOCH_YEAR = 1970;
    static const unsigned int EPOCH_DAYS = 719499;
};

template <> struct Traits<ATMega128_EEPROM>: public Traits<ATMega128_Common>
{
    static const unsigned int SIZE = 4096; // bytes
};

template <> struct Traits<ATMega128_SPI>: public Traits<ATMega128_Common>
{
};

template <> struct Traits<ATMega128_Display>: public Traits<ATMega128_Common>
{
    static const bool on_serial = true;
};

template <> struct Traits<ATMega128_NIC>: public Traits<void>
{
    typedef LIST<Radio> NICS;

    static const unsigned int RADIO_UNITS = NICS::Count<Radio>::Result;
};
#endif


// Abstractions
template <> struct Traits<Thread>: public Traits<void>
{
    static const bool idle_waiting = true;
    static const bool active_scheduler = true;
    static const bool preemptive = true;
    static const bool smp = false;
    static const unsigned int QUANTUM = 10000; // us
};

template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
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
