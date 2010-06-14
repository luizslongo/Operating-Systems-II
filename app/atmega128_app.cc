#include <display.h>
#include <uart.h>
#include <adc.h>
#include <cpu.h>
#include <machine.h>
#include <thread.h>
#include <alarm.h>
#include <timer.h>
#include <sensor.h>

__USING_SYS

OStream cout;
unsigned char count = 1;

int main() {
	ATMega128_Photo_Sensor sensor;
	UART uart;

    CPU::out8(Machine::IO::DDRA, 0xff);
    CPU::out8(Machine::IO::PORTA, ~0);

    union {
    	int s;
    	char b[2];
    } temp;

    while(1) {
		CPU::out8(Machine::IO::PORTA, ~count++);
		Alarm::delay(1000000);
    	temp.s = sensor.sample();
    	uart.put(temp.b[1]);
    	uart.put(temp.b[0]);
    }
}
