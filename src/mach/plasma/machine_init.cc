// EPOS-- PLASMA Mediator Initialization

#include <machine.h>
#include <system.h>
#include <uart.h>

extern "C" { void __epos_library_app_entry(void); }

__BEGIN_SYS

void PLASMA::init()
{
    db<PLASMA>(TRC) << "PLASMA::init()\n";

    // Set all interrupt handlers to "No Handler"
    for(unsigned int i = 0; i < INT_VECTOR_SIZE; i++)
        int_vector(i, int_no_handler);

   //set some valid interrupt handlers
    int_vector(PLASMA_IC::IRQ_UART_READ,  PLASMA_UART::int_handler);
    int_vector(PLASMA_IC::IRQ_UART_WRITE, PLASMA_UART::int_handler);

    Reg32 address;
    ASMV("la    %0, interrupt_service_routine" : "=r" (address) : );
    db<PLASMA>(TRC) << "Address of interrupt dispatcher is " << (void*)address << "\n";

    Reg32 instruction;
    // 1st instruction: or $26, $0, $0  --> 00 00 D0 24
    instruction = 0x0000D024;
    *(unsigned int*)(INT_VECTOR_ADDRESS + 0x00)= (unsigned int)instruction;
    // 2nd instruction: lui $26, hi parte of address  --> 3C 1A cte cte
    instruction = 0x3C1A0000 | (address >> 16);
    *(unsigned int*)(INT_VECTOR_ADDRESS + 0x04) = (unsigned int)instruction;
    //3th instruction: ori $26, $26, lo part of address  --> 37 5A cte cte
    instruction = 0x375A0000 | (address & 0x0000FFFF);
    *(unsigned int*)(INT_VECTOR_ADDRESS + 0x08) = (unsigned int)instruction;
    //4th instruction: jr $26  --> 03 40 00 08
    instruction = 0x03400008;
    *(unsigned int*)(INT_VECTOR_ADDRESS + 0x0C) = (unsigned int)instruction;
    //5th instruction: nop  --> 00 00 00 00 (Delay branch)
    instruction = 0x00000000;
    *(unsigned int*)(INT_VECTOR_ADDRESS + 0x10) = (unsigned int)instruction;

    if(Traits<PLASMA_IC>::enabled)
	PLASMA_IC::init();
    if(Traits<PLASMA_Timer>::enabled)
	PLASMA_Timer::init();
    if(Traits<PLASMA_UART>::enabled)
	PLASMA_UART::init();
    if(Traits<PLASMA_NIC>::enabled)
	PLASMA_NIC::init();
}

__END_SYS
