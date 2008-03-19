// EPOS-- CMAC Init

#include <machine.h>
#include <mach/avr_common/cmac.h>

__BEGIN_SYS

void CMAC::init(unsigned int n) {

    Machine::int_vector(IC::IRQ_TIMER2_COMP, &timer_handler);

    Machine::int_vector(IC::IRQ_SPI_STC, &spi_handler);
    

    _timer.frequency(30);

    _timer.enable();

}

 __END_SYS
