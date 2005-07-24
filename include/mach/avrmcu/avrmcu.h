// EPOS-- AVRMCU Declarations

#ifndef __avrmcu_h
#define __avrmcu_h

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

class AVRMCU: public AVR_Machine
{
private:
    typedef Traits<AVRMCU> Traits;
    static const Type_Id TYPE = Type<AVRMCU>::TYPE;
    

public:
    typedef IO_Map<AVRMCU> IO;

    enum {
	INT_RESET        = 0,
	INT_IRQ0         = 1,
	INT_IRQ1         = 2,
	INT_TIMER2_COMP  = 3,
	INT_TIMER2_OVF   = 4,
	INT_TIMER1_CAPT  = 5,
	INT_TIMER1_COMPA = 6,
	INT_TIMER1_COMPB = 7,
	INT_TIMER1_OVF   = 8,
	INT_TIMER0_OVF   = 9,
	INT_SPI_STC      = 10,
	INT_USART0_RXC   = 11,
	INT_USART0_UDRE  = 12,
	INT_USART0_TXC   = 13,
	INT_ADC          = 14,
	INT_EE_RDY       = 15,
	INT_ANA_COMP     = 16,
	INT_TWI          = 17,
	INT_IRQ2         = 18,
	INT_TIMER0_COMP  = 19,
	INT_SPM_RDY      = 20,

    	INT_TSC 	 = 100, //not implemented
	INT_TIMER 	 = INT_TIMER0_COMP
    };

public:
    AVRMCU(){};

    static Handler::Function * int_handler(int i) {
	if((i < Traits::INT_VEC_SIZE) && (interrupt_vector[i]))
	    return interrupt_vector[i];
	else
	    return 0;
    }
    static void int_handler(int i, Handler::Function * h) {
	if(i < Traits::INT_VEC_SIZE)
	    interrupt_vector[i] = h;
    }
    
    template <Handler::Function * h> static void isr_wrapper(){
    	// Save and restore is performed by the stub function, __vector_handler
    	h();
    }

    static void panic() {
        while(1);
    }


    static int init(System_Info *si);
    
private:

    static Handler::Function * interrupt_vector[Traits<AVRMCU>::INT_VEC_SIZE];

};

typedef AVRMCU Machine;

__END_SYS

#endif
