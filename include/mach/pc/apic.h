// EPOS-- PC Advanced Programmable Interrupt Controller Mediator Declarations

#ifndef __pc_apic_h
#define __pc_apic_h

#include <cpu.h>
#include <mach/pc/memory_map.h>

__BEGIN_SYS

// Intel Pentium APIC (internal, not tested with 82489DX)
class APIC
{
private:
    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:
    // Default mapping addresses
    enum {
	LOCAL_APIC_PHY_ADDR	= 0xfee00000,
	LOCAL_APIC_LOG_ADDR	= Memory_Map<PC>::APIC_MEM,
	IO_APIC_PHY_ADDR	= 0xfec00000,
	IO_APIC_LOG_ADDR	= Memory_Map<PC>::APIC_MEM +
	                          (IO_APIC_PHY_ADDR - LOCAL_APIC_PHY_ADDR)
    };

    // Memory-mapped Registers and Masks
    enum {
	// Local APIC ID Register
	ID			= 0x20,
	ID_SHIFT		= 24,
	ID_MASK			= 0xff000000,

	// Local APIC Version Register
	LVR			= 0x30,

	// End-Of-Interrupt Register
	EOI	   		= 0x0B0,

	// Spurious Interrupt Vector Register
	SVR			= 0xf0,
	SVR_VECTOR		= 0x000000ff,
	SVR_VEC_PROG		= 0x000000f0,
	SVR_VEC_FIX		= 0x0000000f,
	SVR_ENABLE		= 0x00000100,
	SVR_SWDIS		= 0x00000000,
	SVR_SWEN		= 0x00000100,
	SVR_FOCUS		= 0x00000200,
	SVR_FEN			= 0x00000000,
	SVR_FDIS		= 0x00000200,
	SPURIOUS_INT		= 255,

	// In-Service Register (256 bits)
	ISR			= 0x100,

	// Trigger Mode Register (256 bits)
	TMR			= 0x180,

	// Interrupt Request Register (256 bits)
	IRR			= 0x200,

	// Error Status Register
	ESR			= 0x280,
	ESR_SEND_CS_ERROR	= 0x00000001,
	ESR_RECEIVE_CS_ERROR	= 0x00000002,
	ESR_SEND_ACCEPT		= 0x00000004,
	ESR_RECEIVE_ACCEPT	= 0x00000008,
	ESR_SEND_ILLEGAL_VECTOR	= 0x00000020,
	ESR_RECEIVE_ILLEGAL_VECTOR	= 0x00000040,
	ESR_ILLEGAL_REGISTER	= 0x00000080,

 	RESV2_MASK	 	= 0xfff00000,
 	RESV1_MASK	 	= 0x00002000,

	// Interrupt Command Register (64 bits)
	ICR_LOW			= 0x300,
	ICR_HIGH		= 0x310,
	ICRLO_RESV_MASK		= (RESV1_MASK | RESV2_MASK),

	DEST_MASK		= 0x000c0000,
	DEST_DESTFLD		= 0x00000000,
	DEST_SELF		= 0x00040000,
	DEST_ALLISELF		= 0x00080000,
	DEST_ALLESELF		= 0x000c0000,
	DELSTAT_MASK		= 0x00001000,
	DELSTAT_IDLE		= 0x00000000,
	DELSTAT_PEND		= 0x00001000,

	LEVEL_MASK		= 0x00004000,
	LEVEL_DEASSERT		= 0x00000000,
	LEVEL_ASSERT		= 0x00004000,
	TRIGMOD_MASK		= 0x00008000,
	TRIGMOD_EDGE		= 0x00000000,
	TRIGMOD_LEVEL		= 0x00008000,

	DESTMODE_MASK		= 0x00000800,
	DESTMODE_PHY		= 0x00000000,
	DESTMODE_LOG		= 0x00000800,

	DELMODE_INIT		= 0x00000500,
	DELMODE_STARTUP		= 0x00000600,

	// Local Vector Table
	LVT_TIMER		= 0x00000320,
	LVT_THERMAL		= 0x00000330,
	LVT_COUNTER		= 0x00000340,
	LVT_LINT0		= 0x00000350,
	LVT_LINT1		= 0x00000360,
	LVT_ERROR		= 0x00000370,
	INITIAL_COUNT		= 0x00000380,
	CURRENT_COUNT		= 0x00000390,
	DIVIDE_CONF		= 0x000003e0,
	LVT_MASKED		= (1 << 16)

    };

public:
    APIC() {}

    static void remap(Log_Addr addr = LOCAL_APIC_LOG_ADDR) {
	_base = addr;
    }

    static void enable() {
	Reg32 value = read(SVR);
	value &= ~SVR_VECTOR;
	value |= SVR_ENABLE;
	value |= SVR_FOCUS;
	value |= SVR;
	write(SVR, value);
    }
    static void disable() {
	unsigned value  = read(SVR);
	value &= ~SVR_ENABLE;
	write(SVR, value);
    }

    static Reg32 read(unsigned int reg) {
	return *reinterpret_cast<volatile Reg32 *>(_base + reg);
    }
    static void write(unsigned int reg, Reg32 value) {
	*reinterpret_cast<volatile Reg32 *>(_base + reg) = value;
    }

    static int id() {
	return (read(ID) & ID_MASK) >> ID_SHIFT;
    }
    static int version() {
	return read(LVR);
    }

    static void error_handler(const Log_Addr & handler) {
	Reg32 lvt_error = read(LVT_ERROR);
	lvt_error &= 0xffffff00;
	lvt_error |= (handler >> 12) & 0xff;
	write(LVT_ERROR, lvt_error);
    }

    static void ipi_init() {
	// Broadcast INIT IPI to all APs excluding self
	write(ICR_LOW, 0x000c4500); // ***
 	while((read(ICR_LOW) & DELSTAT_MASK));
    }

    static void ipi_start(Log_Addr entry) {
	unsigned int vector = (entry >> 12) & 0xff;

	// Broadcast STARTUP IPI to all APs excluding self twice
 	write(ICR_LOW, 0x000c4600 | vector);
 	while((read(ICR_LOW) & DELSTAT_MASK));
  	for(int i = 0; i < 0x100000; i++); // ***
 	write(ICR_LOW, 0x000c4600 | vector);
 	while((read(ICR_LOW) & DELSTAT_MASK));

	// Give other CPUs a time to wake up (> 10ms)
 	for(int i = 0; i < 0x4000000; i++);
    };

    static void init(Log_Addr addr) {
	// APIC must be on very early in the boot process, so it is
	// subject to memory remappings. We also cannot be sure about 
	// global constructors here
	remap(addr);
	clear();
	enable();
	connect();
    }

    static void init() {
	remap();
	clear();
	enable();
	connect();
    }

private:
    static int maxlvt()	{
	unsigned int v = read(LVR);
	// 82489DXs do not report # of LVT entries
	return (v & 0xf) ? (v >> 16) & 0xff : 2;
    }

    static void clear() {
	int lvts = maxlvt();
	unsigned long v;

	// Masking an LVT entry on a P6 can trigger a local APIC error
	// if the vector is zero. Mask LVTERR first to prevent this
	if(lvts >= 3) {
	    v = 1; // any non-zero vector will do
	    write(LVT_ERROR, v | LVT_MASKED);
	}

	// Careful: we have to set masks only first to deassert
	// any level-triggered sources
	v = read(LVT_TIMER);
	write(LVT_TIMER, v | LVT_MASKED);
	v = read(LVT_LINT0);
	write(LVT_LINT0, v | LVT_MASKED);
	v = read(LVT_LINT1);
	write(LVT_LINT1, v | LVT_MASKED);
	if(lvts >= 4) {
	    v = read(LVT_COUNTER);
	    write(LVT_COUNTER, v | LVT_MASKED);
	}

	// Clean APIC state
	write(LVT_TIMER, LVT_MASKED);
	write(LVT_LINT0, LVT_MASKED);
	write(LVT_LINT1, LVT_MASKED);
	if(lvts >= 3)
	    write(LVT_ERROR, LVT_MASKED);
	if(lvts >= 4)
	    write(LVT_COUNTER, LVT_MASKED);
	v = read(LVR) & 0xff;
	if(v & 0x0f) { // !82489DX
	    if (lvts > 3)
		write(ESR, 0);
	    read(ESR);
	}
    }

    static void connect()	{
	IA32::out8(0x70, 0x22);
	IA32::out8(0x01, 0x23);
    }

    static void disconnect()	{
	IA32::out8(0x70, 0x22);
	IA32::out8(0x00, 0x23);
    }

private:
    static unsigned int _base;
};

__END_SYS

#endif
