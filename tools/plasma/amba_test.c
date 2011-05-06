#include "plasma.h"

#define MemoryRead(A) (*(volatile unsigned int*)(A))
#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)

#define AMBA_BASE 0x00000400
#define AMBA_DEPTH 0x00000100

#define AMBA2_BASE 0x00020000

#define UART_BASE 0x80000000
#define UART_RX_FIFO (UART_BASE+0x0)
#define UART_TX_FIFO (UART_BASE+0x4)
#define UART_TX_STATUS (UART_BASE+0x8)
#define UART_TX_CTRL (UART_BASE+0xC)

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

    MemoryWrite(UART_TX_CTRL, 0x3);//Disable interrupts and clear FIFO

    i = 0;
    while(i < AMBA_DEPTH){
        if (MemoryRead(UART_TX_STATUS) & 0x8)
            continue;
        MemoryWrite(UART_TX_FIFO, i);
        i++;
    }

    while(1);

    return 0;
}

