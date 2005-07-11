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
    	INT_TIMER1_OVF 	= 8,
	INT_TSC   = 100,
	INT_TIMER 	= 19,	
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
    

    static int init(System_Info *si);
    
private:

    static Handler::Function * interrupt_vector[Traits<AVRMCU>::INT_VEC_SIZE];

};

typedef AVRMCU Machine;

__END_SYS

#endif
