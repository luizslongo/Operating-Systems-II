#include "plasma.h"

#define MemoryRead(A) (*(volatile unsigned int*)(A))
#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)

#define AMBA_BASE 0x00000400
#define AMBA_DEPTH 0x00000100

#define AMBA2_BASE 0x00020000

/////////////////////////////
#define UART_BASE 0x80000000
typedef unsigned int uint32_t;
typedef struct {
  //  All elements are 8 bits except for clkdiv (16), but we use uint32 to make
  //    the hardware for decoding easier
  volatile uint32_t clkdiv;  // Set to 50e6 divided by baud rate (no x16 factor)
  volatile uint32_t txlevel; // Number of spaces in the FIFO for writes
  volatile uint32_t rxlevel; // Number of available elements in the FIFO for reads
  volatile uint32_t txchar;  // Write characters to be sent here
  volatile uint32_t rxchar;  // Read received characters here
} uart_regs_t;
#define uart_regs ((uart_regs_t *) UART_BASE)
void
hal_uart_init(void)
{
    uart_regs->clkdiv = 868;  // 57600 bps
}

void
hal_uart_putc(int ch)
{
  if (ch == '\n')       // FIXME for now map \n -> \r\n
    hal_uart_putc('\r');

  while (uart_regs->txlevel == 0)    // wait for fifo to have space
    ;

  uart_regs->txchar = ch;
}

void
hal_uart_putc_nowait(int ch)
{
  if (ch == '\n')       // FIXME for now map \n -> \r\n
    hal_uart_putc('\r');

  if(uart_regs->txlevel)   // If fifo has space
      uart_regs->txchar = ch;
}

int
hal_uart_getc(void)
{
  while ((uart_regs->rxlevel) == 0)  // wait for data to be ready
    ;

  return uart_regs->rxchar;
}
//////////////////////////////


void OS_InterruptServiceRoutine(unsigned int status)
{
   (void)status;
}

int main()
{
    unsigned int i = 0;
    for(i = 0; i < AMBA_DEPTH; ++i){
        MemoryWrite(AMBA_BASE+(i*4), i);
    }

    for(i = 0; i < AMBA_DEPTH; ++i){
        MemoryWrite(AMBA2_BASE+(i*4), MemoryRead(AMBA_BASE+(i*4)));
    }

    hal_uart_init();
    hal_uart_putc('H');
    hal_uart_putc('e');
    hal_uart_putc('l');
    hal_uart_putc('l');
    hal_uart_putc('o');
    hal_uart_putc('!');
    hal_uart_putc('\n');

    while(1){
        hal_uart_putc(hal_uart_getc());
    }


    while(1);

    return 0;
}

