// EPOS-- AVRMCU_IC Test Program

#include <utility/ostream.h>
#include <ic.h>
#include <machine.h>

__USING_SYS

#define PORTB (*(volatile unsigned char *)(0x38)) 
#define DDRB  (*(volatile unsigned char *)(0x37)) 
#define DDRD  (*(volatile unsigned char *)(0x31)) 

volatile unsigned char count;

class Test
{
public:
	Test(){
		Machine::int_handler(1,Machine::handle_wrapper<dummy>);
	}

	static void dummy(void) {
		PORTB = ~count++;
		for(int i = 0; i < 0xffff; i++);
	}
};

int main()
{
	count = 0;
	DDRD = 0x00;
	DDRB = 0xff;
	Test t;
	IC ic;
	ic.enable(IC::IRQ0);
	
	while(1);
}
