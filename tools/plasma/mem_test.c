#include "plasma.h"

int putchar(int value);
int puts(const char *string);

char *itoa10(unsigned long num)
{
   static char buf[12];
   int i;
   buf[10] = 0;
   for (i = 9; i >= 0; --i)
   {
      buf[i] = (char)((num % 10) + '0');
      num /= 10;
   }
   return buf;
}

char *xtoa(unsigned long num)
{
   static char buf[12];
   int i, digit;
   buf[8] = 0;
   for (i = 7; i >= 0; --i)
   {
      digit = num & 0xf;
      buf[i] = digit + (digit < 10 ? '0' : 'A' - 10);
      num >>= 4;
   }
   return buf;
}

#define MemoryRead(A) (*(volatile unsigned int*)(A))
#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)

int main()
{
    puts("\nStarting at 0x10000000, writes and reads in the memory until it gets an error\n");

    unsigned errors = 0;

    unsigned int i = 0;
    for(i = 0; ; ++i){

        if((RAM_EXTERNAL_BASE+(i*4)) >= (RAM_EXTERNAL_BASE+RAM_EXTERNAL_SIZE-1)) break;

        MemoryWrite(RAM_EXTERNAL_BASE+(i*4), i);

        if((i % 1000) == 0){
            puts("Currently At address: ");
            puts(xtoa(RAM_EXTERNAL_BASE+(i*4)));
            puts("\n");

            puts("Write value: ");
            puts(xtoa(i));
            puts("\n");

            puts("Read value: ");
            puts(xtoa(MemoryRead(RAM_EXTERNAL_BASE+(i*4))));
            puts("\n");
        }

        if (i != MemoryRead(RAM_EXTERNAL_BASE+(i*4))){
            puts("Error at address: ");
            puts(xtoa(RAM_EXTERNAL_BASE+(i*4)));
            puts("\n");
            errors += 1;
        }

    }

    puts("\nFinished\nNumber of errors: ");
    puts(xtoa(errors));
    puts("\n");

    while(1);
}

