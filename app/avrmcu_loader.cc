// EPOS-- AVRMCU_LOADER


#include <utility/ostream.h>
#include <display.h>
#include <uart.h>
#include <adc.h>
#include <arch/avr8/cpu.h>

__USING_SYS

OStream cout;

int main() {

  unsigned int count = 0;
  AVR8::io->ddrb=0xff;

  while(1){
    AVR8::io->portb=~count++;
    for(unsigned int i = 0; i < 0xffff; i++);
  }

  return 0;

}

