// EPOS-- PC Timer Mediator Declarations

#ifndef __pc_timer_h
#define __pc_timer_h

#include <cpu.h>
#include <timer.h>

__BEGIN_SYS

// Intel 8253 (i82C54) Timer (tree counters, only counter 0 available to OS)
class i8253
{
private:
    typedef CPU::Reg8 Reg8;

public:
    // The timer's counter
    typedef CPU::Reg16 Count;

    // Clock input 1.193180 MHz (for all 3 channels)
    static const int CLOCK = 1193180;

    // I/O Ports
    enum {
	CNT_0		= 0x40, // Timekeeper
	CNT_1		= 0x41, // Memory refresh
	CNT_2		= 0x42, // Speaker
	CTRL		= 0x43  // Control
    };

    // Control Register Format (8 bits)
    enum {
	SELECT_MASK	= 0xc0, // Select Counter (2 bits)
	SC0		= 0x00, // counter 0
	SC1		= 0x40, // counter 1
	SC2		= 0x80, // counter 2
	RB		= 0xc0, // read-back
	RW_MASK		= 0x30, // Read/Write (2 bits)
	LATCH		= 0x00, // latch counter for reading
	MSB		= 0x10, // read/write MSB only
	LSB		= 0x20, // read/write LSB only
	LMSB		= 0x30,	// read/write LSB then MSB
	MODE_MASK	= 0x0e, // 3 bits
	IOTC		= 0x00, // Interrupt on Terminal Count
	HROS		= 0x02, // Hardware Retriggerable One-Shot
	RG		= 0x04, // Rate generator
	CSSW		= 0x06, // Continuous Symmetrical Square Wave
	STS		= 0x08, // Software Triggered Strobe
	HTS		= 0x0a, // Hardware Triggered Strobe
	COUNT_MODE_MASK	= 0x01, // 1 bit
	BINARY		= 0x00, // Binary count
	BCD		= 0x01, // BCD count
	DEF_CTRL_C0	= SC0	| LMSB	| CSSW	| BINARY, // Counter 0 default
	DEF_CTRL_C1	= SC1	| MSB	| RG	| BINARY, // Counter 1 default
	DEF_CTRL_C2	= SC2	| LMSB	| CSSW	| BINARY  // Counter 2 default
    };

    // Default Counters as set by the BIOS (16 bits)
    enum {
	DEF_CNT_C0	= 0x0000, // CLOCK/65535 ->     18.2 Hz
	DEF_CNT_C1	= 0x0012, // CLOCK/18    ->  66287.8 Hz
	DEF_CNT_C2	= 0x0533, // CLOCK/1331  ->    896.5 Hz
    };

public:
    i8253() {}

    void config(int channel, Count count, bool interrupt) {
	if(channel > 2)
	    return;

	Reg8 cnt, control;
	switch(channel) {
	case(1):
	    cnt = CNT_1;
	    control = DEF_CTRL_C1;
	    break;
	case(2):
	    cnt = CNT_2;
	    control = DEF_CTRL_C2;
	    break;
	default:
	    cnt = CNT_0;
	    control = DEF_CTRL_C0;
	}
//	if(interrupt) control |= IOTC;

	CPU::out8(CTRL, control);
	CPU::out8(cnt, count & 0xff);
	CPU::out8(cnt, count >> 8);
    }

    Count read(int channel) {
	if(channel > 2)
	    return 0;

	Reg8 cnt, control;
	switch(channel) {
	case(1):
	    cnt = CNT_1;
	    control = SC1;
	    break;
	case(2):
	    cnt = CNT_2;
	    control = SC2;
	    break;
	default:
	    cnt = CNT_0;
	    control = SC0;
	}
	control |= LATCH;

	CPU::out8(CTRL, control);
	Count count = CPU::in8(cnt) | (CPU::in8(cnt) << 8);

	return count;
    }

    void reset(int channel) {
        // 8253 doesn't feature a reset, but a counter can be reset by writing
        // the corresponding control register

	if(channel > 2)
	    return;

	Reg8 cnt, ctrl;
	switch(channel) {
	case(1):
	    cnt = CNT_1;
	    ctrl = DEF_CTRL_C1;
	    break;
	case(2):
	    cnt = CNT_2;
	    ctrl = DEF_CTRL_C2;
	    break;
	default:
	    cnt = CNT_0;
	    ctrl = DEF_CTRL_C0;
	}

	Reg8 control = CPU::in8(ctrl);
	Count count = CPU::in8(cnt) | (CPU::in8(cnt) << 8);
	CPU::out8(ctrl, control);
        CPU::out8(cnt, count & 0xff);
        CPU::out8(cnt, count >> 8);
    }
};

class PC_Timer: public Timer_Common, private i8253
{
public:
    PC_Timer() {}

    PC_Timer(const Hertz & f) {
	db<PC_Timer>(TRC) << "PC_Timer(f=" << f << ")\n";
	frequency(f);
	reset();
    }

    Hertz frequency() const { return count2freq(_count); }
    void frequency(const Hertz & f) { 
	_count = freq2count(f);
	reset();
    }

    void reset() {
	db<PC_Timer>(TRC) << "PC_Timer::reset() => {freq=" << frequency()
			  << ",count=" << _count << "}\n";

	config(0, _count, true); 
    }
   
    void enable() { }
    void disable() { }

    Tick read() { return i8253::read(0); }

private:
    static Hertz count2freq(const Count & c) { return CLOCK / c; }
    static Count freq2count(const Hertz & f) { return CLOCK / f; }

private:
    Count _count;
};

__END_SYS

#endif
