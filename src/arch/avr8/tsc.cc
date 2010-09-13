// EPOS AVR8_TSC Implementation

#include <arch/avr8/tsc.h>

__BEGIN_SYS

volatile unsigned long AVR8_TSC::_ts;

__END_SYS

__USING_SYS

#if defined (__atmega1281)

extern "C" void __vector_21(void) __attribute__ ((signal));

extern "C" void __vector_21(void) {

    TSC::_ts++;

}

#elif defined (__atmega128)

extern "C" void __vector_14(void) __attribute__ ((signal));

extern "C" void __vector_14(void) {

    TSC::_ts++;

}

#elif defined (__atmega16)

extern "C" void __vector_8(void) __attribute__ ((signal));

extern "C" void __vector_8(void) {

    TSC::_ts++;

}

#endif
