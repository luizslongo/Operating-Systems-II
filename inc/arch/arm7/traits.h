#ifndef __arm7_traits_h
#define __arm7_traits_h

// Mediators - Architecture - ARM7
__BEGIN_SYS

template <> struct Traits<ARM7>: public Traits<void>
{
};

template <> struct Traits<ARM7_TSC>: public Traits<void>
{
};

template <> struct Traits<ARM7_MMU>: public Traits<void>
{
};

__END_SYS

#endif
