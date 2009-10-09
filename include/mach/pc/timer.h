// EPOS-- PC Timer Mediator Declarations

#ifndef __pc_timer_h
#define __pc_timer_h

#include <cpu.h>
#include <ic.h>
#include <rtc.h>
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

    void config(int channel, Count count, 
		bool interrupt = true, bool periodic = true)
    {
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

// APIC Timer (Local to each CPU on MP configurations)
class APIC_Timer
{
public:
    // The timer's counter
    typedef CPU::Reg32 Count;

    static const unsigned int CLOCK = Traits<PC>::BUS_CLOCK / 16;

public:
    APIC_Timer() {}

    void config(int channel, Count count,
		bool interrupt = true, bool periodic = true) 
    { 
	APIC::config_timer(count, interrupt, periodic);
    }

    void enable(int channel) { APIC::enable_timer(); }
    void disable(int channel) { APIC::disable_timer(); }

    Count read(int channel) { return APIC::read_timer(); }
    
    void reset(int channel) { APIC::reset_timer(); }
};


// PC_Timer
class PC_Timer: public Timer_Common
{
protected:
    typedef IF<Traits<Thread>::smp, APIC_Timer, i8253>::Result Engine;
    typedef Engine::Count Count;

    static const unsigned int CHANNELS = 2;

    static const unsigned int FREQUENCY = Traits<PC_Timer>::FREQUENCY;
    static const unsigned int CLOCK = Engine::CLOCK;
    static const unsigned int COUNT = CLOCK / FREQUENCY;

    typedef int Channel;
    enum {
	SCHEDULER,
	ALARM,
	USER
    };

    PC_Timer(const Hertz & frequency,
	     const Handler * handler,
	     const Channel & channel):
	_channel(channel), _initial(FREQUENCY / frequency),
	_current(_initial), _handler(handler)
    {
	db<Timer>(TRC) << "Timer(f=" << frequency
		       << ",h=" << handler
		       << ",ch=" << channel 
		       << ") => {count=" << _initial << "}\n";

	if(_initial && !_channels[channel]) 
	    _channels[channel] = this;
	else
	    db<Timer>(ERR) << "Timer not installed!\n";
    }

public:
    PC_Timer(const Hertz & frequency,
	     const Handler * handler,
	     const Channel & channel,
	     bool retrigger):
	_channel(channel), _initial(FREQUENCY / frequency),
	_current(_initial), _handler(handler)
    {
	db<Timer>(TRC) << "Timer(f=" << frequency
		       << ",h=" << handler
		       << ",ch=" << channel 
		       << ") => {count=" << _initial << "}\n";

	if(_initial && (unsigned(channel) < CHANNELS) && !_channels[channel])
	    _channels[channel] = this;
	else
	    db<Timer>(WRN) << "Timer not installed!\n";
    }

    ~PC_Timer() {
	db<Timer>(TRC) << "~Timer(f=" << frequency()
		       << ",h=" << _handler
		       << ",ch=" << _channel 
		       << ") => {count=" << _initial << "}\n";

	_channels[_channel] = 0;
    }

    Hertz frequency() const { return (FREQUENCY / _initial); }
    void frequency(const Hertz & f) { _initial = FREQUENCY / f; reset(); }

    Tick read() { return _current; }

    int reset() {
	db<Timer>(TRC) << "Timer::reset() => {f=" << frequency()
		       << ",h=" << _handler
		       << ",count=" << _current << "}\n";

	int percentage = _current * 100 / _initial;
	_current = _initial;

	return percentage;
    }

    void handler(Handler * handler) { _handler = handler; }

    static void enable() { IC::enable(IC::INT_TIMER); }
    static void disable() { IC::enable(IC::INT_TIMER); }

    static int init();

private:
    static Hertz count2freq(const Count & c) { return c ? CLOCK / c : 0; }
    static Count freq2count(const Hertz & f) { return f ? CLOCK / f : 0; }

    static void int_handler(unsigned int irq);

protected:
    unsigned int _channel;
    Count _initial;
    volatile Count _current;
    Handler * _handler;

    static Engine _engine;
    static PC_Timer * _channels[CHANNELS];
};


// Timer used by Alarm
class Alarm_Timer: public PC_Timer
{
public:
    static const unsigned int FREQUENCY = Timer::FREQUENCY;

public:
    Alarm_Timer(const Handler * handler):
	PC_Timer(FREQUENCY, handler, ALARM) {}
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public PC_Timer
{
private:
    typedef RTC::Microsecond Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler * handler): 
	PC_Timer(1000000 / quantum, handler, SCHEDULER) {}
};

__END_SYS

#endif
