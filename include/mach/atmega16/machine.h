// EPOS-- ATMega16 Mediator Declarations

#ifndef __atmega16_h
#define __atmega16_h

#include "../avr_common/machine.h"

__BEGIN_SYS

class ATMega16: public Machine_Common, private AVR_Machine
{
private:
    static const unsigned int INT_VECTOR_SIZE = 24;

public:
    typedef IO_Map<ATMega16> IO;

    typedef void (int_handler)(unsigned int);

public:
    ATMega16(){};

    static int_handler * int_vector(unsigned int i) {
	return (i < INT_VECTOR_SIZE) ? _int_vector[i] : 0;
    }
    static void int_vector(unsigned int i, int_handler * h) {
	if(i < INT_VECTOR_SIZE) _int_vector[i] = h;
    }

    template<typename Dev>
    static Dev * seize(const Type_Id & type, unsigned int unit) {
        //not implemented
	return 0;
    }

    static void release(const Type_Id & type, unsigned int unit) {
        //not implemented
    }

    static void panic() { AVR_Machine::panic(); }
    static void reboot();
    static void poweroff();

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static int init(System_Info * si) { return 0; }
    
private:
    static int_handler * _int_vector[INT_VECTOR_SIZE];
};

__END_SYS

#include "ic.h"
#include "timer.h"
#include "rtc.h"
#include "eeprom.h"
#include "uart.h"
#include "spi.h"
#include <display.h>
#include "nic.h"
#include "adc.h"

#endif
