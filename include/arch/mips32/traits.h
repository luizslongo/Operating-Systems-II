#ifndef __mips32_traits_h
#define __mips32_traits_h

__BEGIN_SYS

// IA32
template <> struct Traits<MIPS32>: public Traits<void>
{
};

template <> struct Traits<MIPS32_TSC>: public Traits<void>
{
};

template <> struct Traits<MIPS32_MMU>: public Traits<void>
{
};

__END_SYS

#endif
