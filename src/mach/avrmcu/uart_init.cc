// EPOS-- AVRMCU_UART Initialization

#include <uart.h>

__BEGIN_SYS

// Class initialization
int AVRMCU_UART::init(System_Info * si)
{
    db<AVRMCU_UART>(TRC) << "AVRMCU_UART::init()\n";
    
    __ucsrc = 0;
    
    unsigned long br = (Traits<CPU>::CLOCK / 8 / B9600) - 1;
    ubrrh(!URSEL & (br>>8));
    ubrrl(br);       
    
    __ucsrc |= (UCSZ1 | UCSZ0); // 8 Data Bits
    __ucsrc &= (!UPM1 & !UPM0); // No Parity
    __ucsrc |= USBS;		// 2 Stop Bits	
    ucsrc(URSEL|__ucsrc);    

    start();
    
    return 0;
}

__END_SYS
