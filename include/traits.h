#ifndef __traits_h
#define __traits_h

#include <system/config.h>

__BEGIN_SYS

template <class Imp>
struct Traits
{
    static const bool debugged = true;
};

class Null_Debug;
template <> struct Traits<Null_Debug>
{
    static const bool error   = false;
    static const bool warning = false;
    static const bool info    = false;
    static const bool trace   = false;
};

class Debug;
template <> struct Traits<Debug>: public Traits<Null_Debug>
{
    static const bool error   = true;
    static const bool warning = true;
    static const bool info    = true;
    static const bool trace   = true;
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

template <> struct Traits<Framework>: public Traits<void>
{
};

// Mediators
template <> struct Traits<IA32>: public Traits<void>
{
  static const int CLOCK = 1000000000;
};

template <> struct Traits<H8>: public Traits<void>
{
  static const long CLOCK = 16000000;
};

template <> struct Traits<PPC32>: public Traits<void>
{
  static const int CLOCK = 200000000;
};

template <> struct Traits<IA32_MMU>: public Traits<void>
{
    static const bool fast_log_to_phy = false;
};

template <> struct Traits<H8_MMU>: public Traits<void>
{
    static const bool fast_log_to_phy = false;
};

template <> struct Traits<AVR8_MMU>: public Traits<void>
{
    static const bool fast_log_to_phy = false;
};

template <> struct Traits<PPC32_MMU>: public Traits<void>
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

template <> struct Traits<PC>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x00008000;
    static const int INT_BASE = 0x20;
    static const int SYSCALL_INT = 0x80;

    static const unsigned int SYSTEM_STACK_SIZE = 4096;
    static const unsigned int SYSTEM_HEAP_SIZE = 4096;
    static const unsigned int APPLICATION_STACK_SIZE = 4096;
    static const unsigned int APPLICATION_HEAP_SIZE = 100 * 4096;
};

template <> struct Traits<RCX>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x8080;	
    static const int INT_BASE = 0x0A;

    static const unsigned int SYSTEM_STACK_SIZE = 512;
    static const unsigned int SYSTEM_HEAP_SIZE = 512;
    static const unsigned int APPLICATION_STACK_SIZE = 512;
    static const unsigned int APPLICATION_HEAP_SIZE = 512;
};

template <> struct Traits<Khomp>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x0180;	
    static const int INT_BASE = 0x00;

    static const unsigned int SYSTEM_HEAP_SIZE = 4096;
    static const unsigned int SYSTEM_STACK_SIZE = 4096;
    static const unsigned int APPLICATION_STACK_SIZE = 4096;
    static const unsigned int APPLICATION_HEAP_SIZE = 4096;
};

template <> struct Traits<AVRMCU>: public Traits<void>
{
    static const unsigned int BOOT_IMAGE_ADDR = 0x0000;
    static const int INT_BASE = 0x00; 

    static const unsigned int SYSTEM_STACK_SIZE = 128;
    static const unsigned int SYSTEM_HEAP_SIZE = 64;
    static const unsigned int APPLICATION_STACK_SIZE = 64;
    static const unsigned int APPLICATION_HEAP_SIZE = 64;
};

template <> struct Traits<PC_PCI>: public Traits<void>
{
    static const int MAX_BUS = 0;
    static const int MAX_DEV_FN = 0xff;
};

template <> struct Traits<PC_Timer>: public Traits<void>
{
    static const int FREQUENCY = 100; // Hz
};

template <> struct Traits<PC_Display>: public Traits<void>
{
    static const int COLUMNS = 80;
    static const int LINES = 25;
    static const int TAB_SIZE = 8;
    static const unsigned int FRAME_BUFFER_ADDRESS = 0xb8000;
};

template <> struct Traits<RCX_Display>: public Traits<void>
{
    static const int COLUMNS = 5;
    static const int LINES = 1;
    static const int TAB_SIZE = 8;
    static const unsigned int FRAME_BUFFER_ADDRESS = 0xef43;
    static const int DIGIT_SEGMENTS = 7;
    static const int SCROLL_DELAY = 2; /* (1)=FAST (2)=MEDIUM (3)=SLOW */
};

// Abstractions
template <> struct Traits<Alarm>: public Traits<void>
{
    static const bool visible = false;
};

template <> struct Traits<Thread>: public Traits<void>
{
    static const bool busy_waiting = false;
    static const bool active_scheduler = true;
    static const unsigned int quantum = 500000; // us
    static const bool preemptive = true;
};

__END_SYS

#endif
