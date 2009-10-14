#ifndef __avr8_traits_h
#define __avr8_traits_h

__BEGIN_SYS

// AVR8
template <> struct Traits<AVR8>: public Traits<void>
{
    static const bool Power_Management = false;
};

template <> struct Traits<AVR8_TSC>: public Traits<void>
{
};

template <> struct Traits<AVR8_MMU>: public Traits<void>
{
};

__END_SYS

#endif
