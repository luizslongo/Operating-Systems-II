// EPOS-- AVRMCU_LOADER


#include <utility/ostream.h>
#include <display.h>
#include <uart.h>
#include <adc.h>
#include <cpu.h>
#include <machine.h>

__USING_SYS

OStream cout;

int main() {
    unsigned int count = 0;
    CPU::out8(Machine::DDRB,0xff);

    while(1){
        CPU::out8(Machine::PORTB,~count++);
	for(unsigned int i = 0; i < 0xffff; i++);
    }

    return 0;

}

