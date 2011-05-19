#include "plasma.h"

#define MemoryRead(A) (*(volatile unsigned int*)(A))
#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)

////////////////////////////////
#define EXT_RAM_BASE 0x10000000
#define EXT_RAM_TOP  0x100FFFFF

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
#define GPIO_BASE 0x80000400

typedef struct {
  volatile uint32_t output;
  volatile uint32_t input;
} gpio_regs_t;

#define gpio_base ((gpio_regs_t *) GPIO_BASE)
//////////////////////////////////


void OS_InterruptServiceRoutine(unsigned int status)
{
   (void)status;
}

void hello(){
    hal_uart_putc('H');
    hal_uart_putc('e');
    hal_uart_putc('l');
    hal_uart_putc('l');
    hal_uart_putc('o');
    hal_uart_putc('!');
    hal_uart_putc('\n');
}


void test_ex_ram(){
    hal_uart_putc('T');
    hal_uart_putc('2');
    hal_uart_putc('-');
    hal_uart_putc('E');
    hal_uart_putc('r');
    hal_uart_putc('a');
    hal_uart_putc('m');
    hal_uart_putc('\n');
}

void test_gpio(){
    hal_uart_putc('T');
    hal_uart_putc('3');
    hal_uart_putc('-');
    hal_uart_putc('G');
    hal_uart_putc('P');
    hal_uart_putc('I');
    hal_uart_putc('O');
    hal_uart_putc('\n');
}

void ok(){
    hal_uart_putc('O');
    hal_uart_putc('K');
    hal_uart_putc('\n');
}

void error(){
    hal_uart_putc('E');
    hal_uart_putc('R');
    hal_uart_putc('R');
    hal_uart_putc('O');
    hal_uart_putc('R');
    hal_uart_putc('\n');
    while(1);
}

int main()
{

    hal_uart_init();

    hello();

    unsigned int i = 0;



    test_ex_ram();
    for(i = EXT_RAM_BASE; i < EXT_RAM_TOP; i += 4){
        MemoryWrite(i, i);
    }
    for(i = EXT_RAM_BASE; i < EXT_RAM_TOP; i += 4){
        if(MemoryRead(i) != i)
            error();
    }
    ok();


    test_gpio();
    while(1){
        gpio_base->output = gpio_base->input;
    }

    return 0;
}

