// EPOS-- AVRMCU Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.


#ifndef __avrmcu_h
#define __avrmcu_h

#include <machine.h>
#include <arch/avr8/cpu.h>
#include <arch/avr8/mmu.h>
#include <mach/avrmcu/ic.h>
#include <mach/avrmcu/display.h>

__BEGIN_SYS

class AVRMCU: public Machine_Common
{
private:
    typedef Traits<AVRMCU> Traits;
    static const Type_Id TYPE = Type<AVRMCU>::TYPE;

public:
	
    enum {
        INT_TIMER       = 0x0000,	
    };

public:
    AVRMCU(){};
    ~AVRMCU(){};
    
    static Handler * interrupt_vector;

    static Handler int_handler(int i) {
	if(i < Traits::INT_VEC_SIZE)
	    return interrupt_vector[i];
    }
    static void int_handler(int i, Handler h) {
	if(i < Traits::INT_VEC_SIZE)
	    interrupt_vector[i] = h;
    }
    
    template <Handler h> static void handler_wrapper(){
    	// Save and restore is done automatically by the stub function,
	// e.g. __vector_0
    	h();
    }    
    

    static int init(System_Info *si);

};

__END_SYS

#endif
