// EPOS-- AVR_EEPROM TEST APPLICATION
// Author: Arliones

#include <cpu.h>
#include <machine.h>
#include <uart.h>
#include <mach/common/avr_eeprom.h>

__USING_SYS


int main() {

    UART u;
    AVR_EEPROM e;
    unsigned char d;

    while(1) {
        u.put('W');
	u.put('a');
	u.put('i');
	u.put('t');
	u.put('\n');

        // Type: 'a' to write a 0xaa / 0x55 pattern
        //       'b' to write a 0x55 / 0xaa pattern
        //       'c' to read the content of the memory and put it on the UART

        d = u.get();

	if(d == 'a') {
	    // Write 0x55 / 0xaa
	    u.put('W');
	    u.put('r');
	    u.put('i');
	    u.put('t');
	    u.put('e');
	    u.put('\n');
	    for(int i = 0; i < 512; i++) {
	        if(i % 2) e.write_byte(i,0x55);
		else e.write_byte(i,0xaa);
	    }
	}
	if(d == 'b') {
	    // Write 0xaa / 0x55
	    u.put('W');
	    u.put('r');
	    u.put('i');
	    u.put('t');
	    u.put('e');
	    u.put('\n');
	    for(int i = 0; i < 512; i++) {
	        if(i % 2) e.write_byte(i,0xaa);
		else e.write_byte(i,0x55);
	    }
	}
	if(d == 'c') {
	    // Put EEPROM content on UART
	    for(int i = 0; i < 512; i++)
	        u.put(e.read_byte(i));
	}
    }
    return 0;
}

