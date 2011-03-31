#include "plasma.h"
#include "uart.h"
#include "elf.h"

#define ENABLE                  0xA5
#define COPY_ADDR		0x100E0000 //Last 128K
#define MEM_BASE		0x10000000
#define MEM_TOP 		0x10100000
#define TOUT			0x00000400

Plasma_UART uart;
volatile unsigned int * leds = (volatile unsigned int *)0x20000030;

void print_string(char * p);
void printhex(int i);
void printhex(short s);
void printhex(char c);

void print_string(char * p)
{
   while(*p){
     if(*p == '\n') uart.put('\r');
     uart.put(*p);
     p++;
   }
}

void printhex(char c)
{
   uart.put("0123456789ABCDEF"[(c >> 4) & 15]);
   uart.put("0123456789ABCDEF"[c & 15]);
}

void printhex(short s){
   printhex((char)(s >> 8));
   printhex((char)(s & 255));
}

void printhex(int i){
   printhex((short)(i >> 16));
   printhex((short)(i & 0x0000FFFF));		   
}

int main( void ) {

  volatile unsigned char * elf_image = (unsigned char *)COPY_ADDR;
  *leds = 0x81;

  print_string("----------------------------------------------------------------\n");
  print_string("                        Plasma ELF Loader\n");
  print_string("          Software and Hardware Integration Lab - UFSC\n");
  print_string("              Hugo Marcondes (hugom@lisha.ufsc.br)\n");
  print_string("                        v0.1 - Sep. 2006\n");
  print_string("----------------------------------------------------------------\n");

  print_string("\n\n");
  print_string("---------------------- Waiting OS image -----------------------\n");
  
  *leds = 0xC3;
  //Synchronize with sender  
  while(uart.get() != ENABLE);
  while(uart.get() != ENABLE);

  unsigned int timeout;
  unsigned int bytes_received = 0;
  bool receiving = true;
  bool waiting = true;
  while(receiving){
    timeout = 0;
    waiting = true;
    while((!uart.has_data()) && waiting){
      *leds = 0xE7;
      if(timeout++ > TOUT){
	  receiving = false; // Assume file finished!
	  waiting = false;
      }
    }
    //Save the byte received 
    if(receiving){
        *leds = 0xFF;
        *elf_image++ = uart.get();
        bytes_received++;
    }
    if((COPY_ADDR + bytes_received) == MEM_TOP){
	print_string("File too long! Aborting receiving!\n");
	*leds = 0x000000AA;
	while(1);
    }
  }

  *leds = bytes_received >> 8;
  print_string("Received 0x");
  printhex((int)bytes_received);
  print_string(" bytes\n");
  
  print_string("\n\n");
  print_string("------------------------ Image Received ------------------------\n");

  print_string("-- File received! Now the ELF Segments will be loaded.\n");

  ELF * elf_file = reinterpret_cast<ELF *>(COPY_ADDR);
  if(!elf_file->valid()) {
    print_string("-- Sorry, the ELF image was corrupted!\n");
    while(1);
  }

  print_string("Number of segments: ");
  printhex((int)elf_file->segments());
  print_string("\n");

  print_string("Segment 0 size: ");
  printhex((int)elf_file->segment_size(0));
  print_string("\n");

  print_string("Segment 0 Addr: ");
  printhex((int)elf_file->segment_address(0));
  print_string("\n");

  print_string("Elf Entry Point: ");
  printhex((int)elf_file->entry());
  print_string("\n");
      

  if(elf_file->load_segment(0) < 0) {
    print_string("-- Sorry, the Code Segment was corrupted!\n");
  }

  for(int i = 1; i < elf_file->segments(); i++) {
      if(elf_file->load_segment(i) < 0) {
        print_string("-- Sorry, the Data Segment was corrupted!\n");
      }
  }

  print_string("-- Job Done! Now the ELF will take control. Bye-Bye :P\n");
  print_string("----------------------------------------------------------------\n");
  print_string("\n\n");

  *leds = 0x00;
  
  // Call setup
  void (*entry)() = reinterpret_cast<void (*)()>(elf_file->entry());
  entry();

  //This point should never be reached ... but ??
  while(1);

  return 0;

};
