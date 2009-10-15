#include <display.h>
#include <uart.h>
#include <adc.h>
#include <cpu.h>
#include <machine.h>
#include <thread.h>
#include <alarm.h>
#include <timer.h>

__USING_SYS

/**
 *  CPU::out8(Machine::IO::PORTB, 0x20) red led
 *  CPU::out8(Machine::IO::PORTB, 0x40) yellow led
 *  CPU::out8(Machine::IO::PORTB, 0x80) green led
 */
int main() {
    int i;

     while (1) {
        for (i = 5; i < 8; i++) {
            CPU::out8(Machine::IO::PORTB, (1 << i));
            Alarm::delay(500000);
            //for(unsigned int j = 0; j < 0xffff; j++);
        }
    }
}
