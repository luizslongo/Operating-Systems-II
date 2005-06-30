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

int main() {

    Display disp;
    unsigned char count = 0;

    CPU::out8(Machine::IO::DDRA, 0x07);
    CPU::out8(Machine::IO::PORTA, ~0);


    Display d;

    while(1) {
	
	d.puts("This is EPOS-- for Mica2\n");
	CPU::out8(Machine::IO::PORTA, ~count++);
	Alarm::delay(1000000);

    }

}
