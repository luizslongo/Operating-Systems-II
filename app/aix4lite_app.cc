// AIX4LITE Dummy Application
//
// Author: Hugo
// Documentation: $EPOS/doc/loader			Date: 23 May 2011

#include <utility/ostream.h>
#include <display.h>
#include <machine.h>

__USING_SYS

OStream cout;

void gpio_handler(unsigned int i){
	cout << "Button(" << i << ") Pressed!\n";
}

int main()
{
  cout << "EPOS LOADED\n";

  CPU::int_disable();
  IC::int_vector(IC::IRQ_GPIO_0, &gpio_handler);	
  IC::enable(IC::IRQ_GPIO_0);
  IC::int_vector(IC::IRQ_GPIO_1, &gpio_handler);	
  IC::enable(IC::IRQ_GPIO_1);
  IC::int_vector(IC::IRQ_GPIO_2, &gpio_handler);	
  IC::enable(IC::IRQ_GPIO_2);
  IC::int_vector(IC::IRQ_GPIO_3, &gpio_handler);	
  IC::enable(IC::IRQ_GPIO_3);
  CPU::int_enable();

  cout << "LEDs are flashing now!!!\n";
  unsigned int * led = (unsigned int *)(Traits<Machine>::LEDS_ADDRESS);
  for (unsigned int i = 0; i < 0x100 ; i++) {
    *led = i;
    for(unsigned int x = 0; x < 0x0007FFFF; x++);
  }
  cout << "I'm done ... CPU should halt now!\n";

  while(1);
	
  return 0;
}

