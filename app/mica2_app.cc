#include <utility/ostream.h>
#include <display.h>
#include <uart.h>
#include <adc.h>
#include <cpu.h>
#include <machine.h>
#include <thread.h>
#include <alarm.h>
#include <timer.h>

__USING_SYS

char * string = "Um caracter por segundo.\n";

int main() {

    UART u;

    char * ptr = string;

    unsigned char c;
    
    unsigned int count = 0;

//    u.send_byte('A');

    Timer t;

    CPU::out8(Machine::IO::DDRA, 0x07);
    CPU::out8(Machine::IO::PORTA, ~0);

    while(1) {

	char * ptr = string;

	while(*ptr != '\0'){

	    CPU::out8(Machine::IO::PORTA, ~(count++));
	    Alarm::delay(1000000);
	    if(count == 8) count = 0; 
	    if(u.receive_byte(&c) == 1)
		u.send_byte(c);
	    else
		u.send_byte(*ptr++);

	}
	u.send_byte(0x0d);
	u.send_byte(0x0a);
    }
}

/*
const int iterations = 100;

int func_a(void);
int func_b(void);

Thread * a;
Thread * b;
Thread * m;

int main()
{
    CPU::int_disable();
    CPU::out8(Machine::IO::DDRA,0x07);
    CPU::out8(Machine::IO::PORTA,~0x03);

    m = Thread::self();

    a = new Thread(&func_a);
    b = new Thread(&func_b);
    
    CPU::out8(Machine::IO::PORTA,~0x05);

    a->pass();

    delete a;
    delete b;
    delete m;
    
    CPU::out8(Machine::IO::PORTA,~0x07);
	
    while(1);

    return 0;
}

int func_a(void)
{
    for(int i = iterations; i > 0; i--) {
	for(int i = 0; i < 29; i++){
	    CPU::out8(Machine::IO::PORTA,~0x01);
	}
	b->pass();
    }
    b->pass();
}

int func_b(void)
{
    for(int i = iterations; i > 0; i--) {
        for(int i = 0; i < 29; i++){
	    CPU::out8(Machine::IO::PORTA,~0x02);
	}
	a->pass();
    }
    m->pass();
}
*/

/*

#include <utility/ostream.h>
#include <display.h>
#include <uart.h>
#include <adc.h>
#include <cpu.h>
#include <machine.h>
#include <thread.h>

__USING_SYS


int main()
{
    UART u;
    unsigned char count;
    CPU::out8(Machine::IO::DDRA,0x07);
    u.send_byte('A');
    u.send_byte('A');
    u.send_byte('A');
    u.send_byte('A');
    u.send_byte('A');
    unsigned char c;
    while(1) {
	if (u.receive_byte(&c) == 1) {
	    u.send_byte(c);
	    CPU::out8(Machine::IO::PORTA,~count++);
	    if (count > 3) count = 0;
	    for(unsigned int i = 0; i < 0xffff; i++);
	}
    }
}


*/
