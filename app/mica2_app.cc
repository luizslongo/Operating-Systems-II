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

    Timer t;

    CPU::out8(Machine::IO::DDRA, 0x07);
    CPU::out8(Machine::IO::PORTA, ~0);

    while(1) {

	char * ptr = string;

	while(*ptr != '\0'){

	    CPU::out8(Machine::IO::PORTA, ~(count++));
	    Alarm::delay(1000000);
	    if(count == 8) count = 0; 
	    u.put(*ptr++);

	}
	u.put(0x0d);
	u.put(0x0a);
    }
}
