#ifndef __ia32_traits_h
#define __ia32_traits_h

__BEGIN_SYS

// IA32
template <> struct Traits<IA32>: public Traits<void>
{
};

template <> struct Traits<IA32_TSC>: public Traits<void>
{
};

template <> struct Traits<IA32_MMU>: public Traits<void>
{
};

__END_SYS

#endif
