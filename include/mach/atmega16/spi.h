// EPOS-- ATMega16 SPI Mediator Declarations

#ifndef __atmega16_spi_h
#define __atmega16_spi_h

#include "../avr_common/spi.h"

__BEGIN_SYS

class ATMega16_SPI: public SPI_Common, private AVR_SPI
{
public:
    ATMega16_SPI() {}

    char get() { return AVR_SPI::get(); }
    void put(char c) { AVR_SPI::put(c); }

    static int init(System_Info * si) { return 0; }
};

__END_SYS

#endif
