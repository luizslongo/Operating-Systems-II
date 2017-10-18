// EPOS IA32 Architecture Metainfo

#ifndef __ia32_traits_h
#define __ia32_traits_h

#include <system/config.h>

__BEGIN_SYS

template<> struct Traits<CPU>: public Traits<void>
{
    enum {LITTLE, BIG};
    static const unsigned int ENDIANESS         = LITTLE;
    static const unsigned int WORD_SIZE         = 32;
    static const unsigned int CLOCK             = 2000000000;
    static const bool unaligned_memory_access   = true;
};

template<> struct Traits<TSC>: public Traits<void>
{
};

template<> struct Traits<MMU>: public Traits<void>
{
    static const bool colorful = false;
    static const unsigned int COLORS = 1;
};

template<> struct Traits<FPU>: public Traits<void>
{
    static const bool enabled = false;
};

template<> struct Traits<PMU>: public Traits<void>
{
    static const bool enabled = true;
    enum { V1, V2, V3, DUO, MICRO, ATOM, SANDY_BRIDGE };
    static const unsigned int VERSION = SANDY_BRIDGE;

    static const bool pmc_interrupt_enabled = true;
    static const unsigned int pmc_handler_period = 500000;
};

__END_SYS

#endif
