// EPOS RISC-V 64 Architecture Metainfo
#ifndef __riscv64_traits_h
#define __riscv64_traits_h

#include <system/config.h>

__BEGIN_SYS

template<> struct Traits<CPU>: public Traits<Build>
{
    enum {LITTLE, BIG};
    static const unsigned int ENDIANESS         = LITTLE;
    static const unsigned int WORD_SIZE         = 64;
    static const unsigned int CLOCK             = 50000000; // maybe
    static const bool unaligned_memory_access   = false;
};

template<> struct Traits<MMU>: public Traits<Build>
{
    static const bool colorful = false;
    static const unsigned int COLORS = 1;
};

template<> struct Traits<TSC>: public Traits<Build>
{
    static const bool enabled = false;
};

__END_SYS

#endif
