// EPOS-- Common Declarations for AVR Machines

#ifndef __avr_machine_h
#define __avr_machine_h

#include <machine.h>
#include <utility/handler.h>

__BEGIN_SYS

class AVR_Machine: public Machine_Common
{

public:
    
    template <Handler::Function * h> static void handler_wrapper(){
    	// Save and restore is performed by the stub function, __vector_handler
    	h();
    }

    static void panic() {
        while(1);
    }


};

__END_SYS

#endif
