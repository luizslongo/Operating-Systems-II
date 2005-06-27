// EPOS-- Mica2 Declarations

#ifndef __mica2_h
#define __mica2_h

#include <machine.h>
#include <mach/mica2/io_map.h>
#include <arch/avr8/cpu.h>
#include <arch/avr8/mmu.h>
#include <mach/mica2/ic.h>
#include <mach/mica2/adc.h>
#include <mach/mica2/uart.h>
#include <mach/mica2/display.h>
#include <utility/handler.h>

__BEGIN_SYS

class Mica2: public Machine_Common
{
private:
    typedef Traits<Mica2> Traits;
    static const Type_Id TYPE = Type<Mica2>::TYPE;


public:

    typedef IO_Map<Mica2> IO;

    enum {
	INT_RESET        = 0,
	INT_IRQ0         = 1,
	INT_IRQ1         = 2,
	INT_IRQ2         = 3,
	INT_IRQ3         = 4,
	INT_IRQ4         = 5,
	INT_IRQ5         = 6,
	INT_IRQ6         = 7,
	INT_IRQ7         = 8,
	INT_TIMER2_COMP  = 9,
	INT_TIMER2_OVF   = 10,
	INT_TIMER1_CAPT  = 11,
	INT_TIMER1_COMPA = 12,
	INT_TIMER1_COMPB = 13,
	INT_TIMER1_OVF   = 14,
	INT_TIMER0_COMP  = 15,
	INT_TIMER0_OVF   = 16,
	INT_SPI_STC      = 17,
	INT_USART0_RXC   = 18,
	INT_USART0_UDRE  = 19,
	INT_USART0_TXC   = 20,
	INT_ADC          = 21,
	INT_EE_RDY       = 22,
	INT_ANA_COMP     = 23,
	INT_TIMER1_COMPC = 24,
	INT_TIMER3_CAPT  = 25,
	INT_TIMER3_COMPA = 26,
	INT_TIMER3_COMPB = 27,
	INT_TIMER3_COMPC = 28,
	INT_TIMER3_OVF   = 29,
	INT_USART1_RXC   = 30,
	INT_USART1_UDRE  = 31,
	INT_USART1_TXC   = 32,
	INT_TWI          = 33,
	INT_SPM_RDY      = 34,
    	INT_TSC 	 = INT_TIMER1_OVF,	
	INT_TIMER 	 = INT_TIMER0_COMP	
    };


public:
    Mica2(){};

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
    
    template <Handler::Function * h> static void handler_wrapper(){
    	// Save and restore is performed by the stub function, __vector_handler
    	h();
    }

    static void panic() {
        while(1);
    }

    static int init(System_Info *si);
    
private:

    static Handler::Function * interrupt_vector[Traits<Mica2>::INT_VEC_SIZE];

};

typedef Mica2 Machine;

__END_SYS

#endif
