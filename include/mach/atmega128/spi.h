// EPOS-- ATMega128 SPI Mediator Declarations

#ifndef __atmega128_spi_h
#define __atmega128_spi_h

#include "../avr_common/spi.h"

__BEGIN_SYS

class ATMega128_SPI: public SPI_Common, private AVR_SPI
{
public:
    ATMega128_SPI() {}

    char get() { return AVR_SPI::get(); }
    void put(char c) { AVR_SPI::put(c); }

    static int init(System_Info * si) { return 0; }
};

__END_SYS

#endif
