// EPOS AVRMCU_UART Test Program

//#include <utility/ostream.h>
#include <uart.h>
//#include <framework.h>

__USING_SYS


#define PORTB (*(volatile unsigned char *)(0x38)) 
#define DDRB  (*(volatile unsigned char *)(0x37)) 

int main()
{

	UART uart;
	unsigned char c;

	DDRB = 0xff;
	PORTB = 0x33;
	
	while(1);

	while (true)
		if (uart.receive_byte(&c) == 1)	{
			uart.send_byte(c);
			PORTB = ~c;
		}	
		
	return 0;
}
