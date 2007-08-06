// EPOS-- PLASMA Interrupt Controller Mediator Declarations

#ifndef __plasma_ic_h
#define __plasma_ic_h

#include <cpu.h>
#include <ic.h>

__BEGIN_SYS

class PLASMA_IC: public IC_Common
{
private:
    typedef CPU::Reg32 Mask;

    static const unsigned int IC_MASK_ADDRESS = Traits<PLASMA_IC>::MASK_ADDRESS;
    static const unsigned int IC_STATUS_ADDRESS = Traits<PLASMA_IC>::STATUS_ADDRESS;


public:

    enum {
	IRQ_UART_READ	= 0,
	IRQ_UART_WRITE	= 1,
	IRQ_NO_TIMER	= 2,
	IRQ_TIMER	= 3,
	IRQ_NO_GPIO30	= 4,
	IRQ_NO_GPI031	= 5,
	IRQ_GPIO30	= 6,
	IRQ_GPI031	= 7
    };

    enum IC_STATUS_REG {
	IC_UART_READ_AVAILABLE 		= 0x01,
	IC_UART_WRITE_AVAILABLE		= 0x02,
	IC_COUNTER18_NOT		= 0x04,
	IC_COUNTER18			= 0x08,
	IC_GPIO30_NOT			= 0x10,
	IC_GPIO31_NOT			= 0x20,
	IC_GPIO30			= 0x40,
	IC_GPIO31			= 0x80
    };

    static const unsigned int DEFAULT_ENABLE_INTR_MASK = IC_UART_READ_AVAILABLE | IC_UART_WRITE_AVAILABLE | IC_COUNTER18;

public:

    PLASMA_IC() {}

    static void enable(IRQ irq) {
	unsigned int maskreg = *(volatile unsigned int*)IC_MASK_ADDRESS;
       *(volatile unsigned int*)IC_MASK_ADDRESS = maskreg | (1<<irq);
    }

    static void mask(unsigned int mask) {
        db<PLASMA_IC>(TRC) << "PLASMA_IC::mask(mask=" << mask << ")\n";
       *(volatile unsigned int*)IC_MASK_ADDRESS = mask;
    }

    static void disable() {
        db<PLASMA_IC>(TRC) << "PLASMA_IC Disabling all IC interrupts!\n";
	*(volatile unsigned int*)IC_MASK_ADDRESS = 0x00;
    }

    static void disable(IRQ irq) {
	unsigned int mask = *(volatile unsigned int*)IC_MASK_ADDRESS;
        *(volatile unsigned int*)IC_MASK_ADDRESS = mask & ~(1<<irq);
    }

    static void int_handler(unsigned int interrupt); 

    static void init();

private:

};

__END_SYS

#endif
