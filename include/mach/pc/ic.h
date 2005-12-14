// EPOS-- PC Interrupt Controller Mediator Declarations

#ifndef __pc_ic_h
#define __pc_ic_h

#include <ic.h>

__BEGIN_SYS

// Intel 8259A Interrupt Controller (master and slave are seen as a unit)
class i8259A
{
private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;

public:
    // I/O Ports
    enum {
	MASTER_CMD	= 0x20,
	MASTER_IMR	= 0x21,
	SLAVE_CMD	= 0xa0,
	SLAVE_IMR	= 0xa1
    };

    // Commands
    enum {
	SELECT_IRR	= 0x0a,
	SELECT_ISR	= 0x0b,
	ICW1		= 0x11, // flank, cascaded, more ICWs
	ICW4		= 0x01,
	EOI		= 0x20
    };

    // Cascading
    enum {
	IRQ_CASCADE = 2
    };

public:
    i8259A() {}

    static void remap(Reg8 base) { // Reconfigure
	// Configure Master PIC
	IA32::out8(MASTER_CMD, ICW1);
	IA32::out8(MASTER_IMR, base);              // ICW2 is the base
	IA32::out8(MASTER_IMR, 1 << IRQ_CASCADE);  // ICW3 = IRQ2 cascaded
	IA32::out8(MASTER_IMR, ICW4);

	// Configure Slave PIC
	IA32::out8(SLAVE_CMD, ICW1);	
	IA32::out8(SLAVE_IMR, base + 8); // ICW2 is the base	
	IA32::out8(SLAVE_IMR, 0x02);     // ICW3 = cascaded from IRQ1	
	IA32::out8(SLAVE_IMR, ICW4);  
    }

    static Reg16 irr() { // Pending interrupts
	IA32::out8(MASTER_CMD, SELECT_IRR);
	IA32::out8(SLAVE_CMD, SELECT_IRR);
	return IA32::in8(MASTER_CMD) | (IA32::in8(SLAVE_CMD) << 8);
    }

    static Reg16 isr() { // In-service interrupts
	IA32::out8(MASTER_CMD, SELECT_ISR);
	IA32::out8(SLAVE_CMD, SELECT_ISR);
	return IA32::in8(MASTER_CMD) | (IA32::in8(SLAVE_CMD) << 8);
    }

    static Reg16 imr() { // Interrupt mask
	return IA32::in8(MASTER_IMR) | (IA32::in8(SLAVE_IMR) << 8);
    }

    static void imr(Reg16 mask) {
	IA32::out8(MASTER_IMR, mask);
	IA32::out8(SLAVE_IMR, mask >> 8);
    }

    static void eoi() { // End of interrupt
	if(isr() & (1 << IRQ_CASCADE)) // was it an slave PIC interrupt?
	    IA32::out8(SLAVE_CMD, EOI);
	IA32::out8(MASTER_CMD, EOI); // always send EOI to master
    }
};

class PC_IC: public IC_Common, private i8259A
{
private:
    static const unsigned int HARD_INT = Traits<PC>::HARDWARE_INT_OFFSET;

public:
    // IRQs
    static const unsigned int IRQS = 16;
    enum {
	IRQ_TIMER	= 0,
	IRQ_KEYBOARD	= 1,
	IRQ_CASCADE	= 2
    };

public:
    PC_IC() {}

    static void enable(IRQ irq) { imr(imr() & ~(1 << irq)); }
    static void disable() { imr(~(1 << IRQ_CASCADE)); }
    static void disable(IRQ irq) { imr(imr() | (1 << irq)); }

    static int init(System_Info * si);
};

__END_SYS

#endif
