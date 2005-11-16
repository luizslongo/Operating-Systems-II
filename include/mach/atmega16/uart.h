// EPOS-- ATMega16_UART Declarations

#ifndef __atmega16_uart_h
#define __atmega16_uart_h

#include <uart.h>
#include "memory_map.h"
#include "../common/avr_uart.h"

__BEGIN_SYS

class ATMega16_UART: public AVR_UART
{
private:
    typedef Traits<ATMega16_UART> _Traits;
    static const Type_Id TYPE = Type<ATMega16_UART>::TYPE; 

public:
    ATMega16_UART(unsigned int unit = 0) : AVR_UART(9600,8,0,1,unit) {}
    ATMega16_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
		unsigned int stop_bits, unsigned int unit = 0) 
	: AVR_UART(baud, data_bits, parity, stop_bits,unit) {}

    static int init(System_Info *si); 

};

typedef ATMega16_UART UART;

__END_SYS

#endif
