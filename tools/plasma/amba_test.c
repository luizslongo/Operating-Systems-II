#include "plasma.h"

#define MemoryRead(A) (*(volatile unsigned int*)(A))
#define MemoryWrite(A,V) *(volatile unsigned int*)(A)=(V)

#define AMBA_BASE 0x00000400
#define AMBA_DEPTH 0x00000100

#define AMBA2_BASE 0x00020000

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

    while(1);

    return 0;
}

