// EPOS-- Mica2_UART Declarations

#ifndef __mica2_uart_h
#define __mica2_uart_h

#include <uart.h> 
#include "memory_map.h"
#include "../common/avr_uart.h"

__BEGIN_SYS

class Mica2_UART: public AVR_UART
{
private:
    typedef Traits<Mica2_UART> _Traits;
    static const Type_Id TYPE = Type<Mica2_UART>::TYPE; 

public:
    Mica2_UART(unsigned int unit = 0) : AVR_UART(9600,8,0,1,unit) {}
    Mica2_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
	       unsigned int stop_bits, unsigned int unit = 0) 
	: AVR_UART(baud, data_bits, parity, stop_bits,unit) {}

    static int init(System_Info *si); 

};

typedef Mica2_UART UART;

__END_SYS

#endif



