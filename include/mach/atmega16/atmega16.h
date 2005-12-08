// EPOS-- ATMega16 Declarations

#ifndef __atmega16_h
#define __atmega16_h

#include <machine.h>
#include "../common/avr_machine.h"
#include <arch/avr8/cpu.h>
#include <arch/avr8/mmu.h>
#include "ic.h"
#include "memory_map.h"
#include "adc.h"
#include "uart.h"
#include "display.h"
#include "sensor.h"
#include <utility/handler.h>

__BEGIN_SYS

class ATMega16: public AVR_Machine
{
private:
    typedef Traits<ATMega16> Traits;
    static const Type_Id TYPE = Type<ATMega16>::TYPE;
    

public:

    typedef IO_Map<ATMega16> IO;

public:
    typedef void (int_handler)(unsigned int);

public:
    ATMega16(){};

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

    static int_handler * interrupt_vector[Traits<ATMega16>::INT_VEC_SIZE];

};

typedef ATMega16 Machine;

__END_SYS

#endif
