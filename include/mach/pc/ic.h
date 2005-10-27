// EPOS-- PC Interrupt Controller Mediator Declarations

#ifndef __pc_ic_h
#define __pc_ic_h

#include <ic.h>

__BEGIN_SYS

class PC_IC: public IC_Common
{
private:
    typedef Traits<PC_IC> Traits;
    static const Type_Id TYPE = Type<PC_IC>::TYPE;

    static const unsigned int HARD_INT = __SYS(Traits)<PC>::HARD_INT;

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;

    // 8259A I/O Ports
    enum {
	MASTER_CMD	= 0x20,
	MASTER_IMR	= 0x21,
	SLAVE_CMD	= 0xa0,
	SLAVE_IMR	= 0xa1
    };

    // 8259A Commands
    enum {
	SELECT_IRR	= 0x0a,
	SELECT_ISR	= 0x0b,
	ICW1		= 0x11, // flank, cascaded, more ICWs
	ICW4		= 0x01,
	EOI		= 0x20
    };

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

    static void enable() { imr(0); }
    static void enable(IRQ irq) { imr(imr() & ~(1 << irq)); }
    static void disable() { imr(0xffff); }
    static void disable(IRQ irq) { imr(imr() | (1 << irq)); }

    // 8259A specific methods

    static void remap(Reg8 base) { // Reconfigure
	// Configure Master PIC
	IA32::out8(MASTER_CMD, ICW1);
	IA32::out8(MASTER_IMR, base); // ICW2 is the base
	IA32::out8(MASTER_IMR, 0x04); // ICW3 = IRQ2 cascaded
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
	if(isr() & 0x0004) // was it an slave PIC interrupt?
	    IA32::out8(SLAVE_CMD, EOI);
	IA32::out8(MASTER_CMD, EOI); // always send EOI to master
    }

    static int init(System_Info * si);
};

typedef PC_IC IC;

__END_SYS

#endif
