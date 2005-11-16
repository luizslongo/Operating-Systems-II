// EPOS-- ATMega128 Declarations

#ifndef __atmega128_h
#define __atmega128_h

#include <machine.h>
#include "../common/avr_machine.h"
#include <arch/avr8/cpu.h>
#include <arch/avr8/mmu.h>
#include <mach/atmega128/ic.h>
#include <mach/atmega128/adc.h>
#include <mach/atmega128/sensor.h>
#include <mach/atmega128/uart.h>
#include <mach/atmega128/display.h>
#include <utility/handler.h>

__BEGIN_SYS

class ATMega128: public AVR_Machine
{
private:
    typedef Traits<ATMega128> Traits;
    static const Type_Id TYPE = Type<ATMega128>::TYPE;


public:

    typedef IO_Map<ATMega128> IO;

public:
    typedef void (int_handler)(int);

public:
    ATMega128(){};

    static int_handler * int_vector(int i) {
	if((i < Traits::INT_VEC_SIZE) && (interrupt_vector[i]))
	    return interrupt_vector[i];
	else
	    return 0;
    }
    static void int_vector(int i, int_handler * h) {
	if(i < Traits::INT_VEC_SIZE)
	    interrupt_vector[i] = h;
    }

    template<typename Dev>
    static Dev * seize(const Type_Id & type, unsigned int unit) {
        //not implemented
    }

    static void release(const Type_Id & type, unsigned int unit) { 
        //not implemented
    }

    template <Handler::Function * h> static void isr_wrapper(){
    	// Save and restore is performed by the stub function, __vector_handler
    	h();
    }

    static void panic() {
        while(1);
    }

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static int init(System_Info *si);
    
private:

    static int_handler * interrupt_vector[Traits<ATMega128>::INT_VEC_SIZE];

};

typedef ATMega128 Machine;

__END_SYS

#endif
