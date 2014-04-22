#ifndef __panda_timer_h
#define __panda_timer_h

#include <timer.h>
#include <ic.h>
#include <system/meta.h>

__BEGIN_SYS
const bool zynq = Traits<Zynq>::enabled;

class PandaBoard_Timer: public Timer_Common
{
protected:

    static const unsigned int PRIVATE_TIMER_BASE = 0x48240600;
    
    // Timer and Watchdog registers - based on the PRIVATE_TIMER_BASE address
    enum {
        PTLR    = 0x00, //Private Timer Load Register
        PTCTR   = 0x04, //Private Timer Counter Register
        PTCLR   = 0x08, //Private Timer Control Register
        PTISR   = 0x0C, //Private Timer Interrupt Status Register
        WLR     = 0x20, //Watchdog Load Register
        WCTR    = 0x24, //Watchdog Counter Register
        WCLR    = 0x28, //Watchdog Control Register
        WISR    = 0x2C, //Watchdog Interrupt Status Register
        WRSR    = 0x30, //Watchdog Reset Status Register
        WDR     = 0x34, //Watchdog Disable Register
    };
    
    enum {
        INTERRUPT_CLEAR     = 1,
        TIMER_ENABLE        = 1,
        TIMER_AUTO_RELOAD   = 2, //periodic
        TIMER_IT_ENABLE     = 4,
        TIMER_WD_MODE       = 8,
        TIMER_PRESCALE_SHIFT = 8,        
    };

    static const CPU::Reg8 IRQ = IC::TIMERINT0;
    
   
    static void load(CPU::Reg32 val) {
        CPU::out32(PRIVATE_TIMER_BASE + PTLR, val);
    }

    static CPU::Reg32 load() {
        return CPU::in32(PRIVATE_TIMER_BASE + PTLR);
    }    
	
    static void control(CPU::Reg32 val) {
        CPU::out32(PRIVATE_TIMER_BASE + PTCLR, val);
    }

    static CPU::Reg32 control() {
        return CPU::in32(PRIVATE_TIMER_BASE + PTCLR);
    }

    static CPU::Reg8 irq() { return IRQ /*+ _channel*/; }

    static void status(CPU::Reg32 val) {
        CPU::out32(PRIVATE_TIMER_BASE + PTISR, val);
    }

    static CPU::Reg32 status() {
        return CPU::in32(PRIVATE_TIMER_BASE + PTISR);
    }


public:
    typedef unsigned long Hertz;
    typedef CPU::Reg32 Count;   
    typedef short Channel;
    enum {
        TSC,
        ALARM,
        SCHEDULER,        
    };
protected:
    static Handler * _handler[3];
    Channel _channel;
public:

    static const unsigned int CLOCK = Traits<Machine>::CLOCK;
    static const unsigned int FREQUENCY = 1000;
    
    PandaBoard_Timer(const Hertz & freq,
         const Handler * hand,const Channel & channel) :
         _channel(channel)
    {
        db<Timer>(TRC) << "Timer(chan="<<channel<<
                          ",hdl="<<(void*)hand<<
                          ",freq="<<freq<<")\n";                
       /*
		* The timer is incremented every prescaler value+1.
		* For example, if the prescaler has a value of five then
		* the global timer is incremented every six clock cycles.
		* PERIPHCLK (clock/2) is the reference clock for this.
		* */ 
        unsigned int prescale = (CLOCK / 2) / freq;
        if(prescale > 0)
            --prescale;
        
        //prescaler is limited to 1 byte
        if(prescale > 0xFF)
            prescale = 0xFF;
            
        frequency(freq);
        handler(hand);
        value(freq2count(freq));  
        
        control(control() | (prescale << TIMER_PRESCALE_SHIFT));
        
        enable();
    }

    void handler(Handler * hand) {          
        _handler[_channel] = hand;        
    }

    Hertz frequency() { 
        if (_channel == 0)
            return CLOCK / load();
        else
            return FREQUENCY / load();
    }
    
    void frequency(const Hertz & f) {
        db<Timer>(TRC) << "Timer_"<<_channel<<"::frequency(f="<<f<<")\n";
        if (_channel == 0)
            load(CLOCK / f);
        else
            load(FREQUENCY / f);
    };

    static void init();

    static void int_handler(IC::Interrupt_Id id);

    void reset() {
        db<IC>(TRC) << "Timer_"<<_channel<<"::reset()\n";
        value(load());
    }
    
    void enable(){
        db<IC>(TRC) << "Timer_"<<_channel<<"::enable()\n";
        control(control() | TIMER_ENABLE);        
    }

    void disable() {
        db<IC>(TRC) << "Timer_"<<_channel<<"::disable()\n";
        control(control() & ~(TIMER_ENABLE));
    }

    Tick read() {
        return value();
    }
    
    static void value(CPU::Reg32 val) {
        CPU::out32(PRIVATE_TIMER_BASE + PTCTR, val);
    }

    static CPU::Reg32 value() {
        return CPU::in32(PRIVATE_TIMER_BASE + PTCTR);
    }

private:
    static Hertz count2freq(const Count & c) {
	    return c ? CLOCK / c : 0;
    }

    static Count freq2count(const Hertz & f) { 
	    return f ? CLOCK / f : 0;
    }


};

class Zynq_Timer: public PandaBoard_Timer
{
    static const unsigned int PRIVATE_TIMER_BASE = 0xf8f00600;
	typedef unsigned int Microsecond;
	public:
	Zynq_Timer(const Hertz& freq, const Handler* hand, const Channel& channel)
		: PandaBoard_Timer(freq,hand,channel)
	{
		_channel = channel;
        frequency(freq);
        handler(hand);
        
        enable();
	}
    void frequency(const Hertz& f)
	{
        unsigned int prescale = (CLOCK / 2) / f;
		prescale = prescale > 0xff? 0xff : prescale;
        control(control() | (prescale << TIMER_PRESCALE_SHIFT));
	}
	Hertz frequency()
	{
		unsigned int prescale = (control() >> TIMER_PRESCALE_SHIFT) & 0xff;
		return (CLOCK/2)/prescale;
	}

	/*
	enum{
		GLOBAL_TIMER_COUNTER_LO 		= 0xf8f00200,
		GLOBAL_TIMER_COUNTER_HI 		= 0xf8f00204,
		GLOBAL_TIMER_CONTROL			= 0xf8f00208,
		GLOBAL_TIMER_INTERRUPT_STATUS	= 0xf8f0020c,
		COMPARATOR_VALUE_LO				= 0xf8f00210,
		COMPARATOR_VALUE_HI				= 0xf8f00214,
		AUTO_INCREMENT					= 0xf8f00218,
		PRIVATE_TIMER_LOAD				= 0xf8f00600,
		PRIVATE_TIMER_COUNTER			= 0xf8f00604,//Issue interrupt 29 when reaches 0.
		PRIVATE_TIMER_CONTROL			= 0xf8f00608,
		PRIVATE_TIMER_INTERRUPT_STATUS	= 0xf8f0060c,
		
		TIMER_ENABLE					= 1,
		AUTO_RELOAD						= 1<<1,
		IRQ_ENABLE						= 1<<2,
		PRESCALE_OFFSET					= 8,
	};
	//Note: The private timers stop counting when the associated processor is in debug state.
	//Writing to the Timer Load Register also writes to the Timer Counter Register.
	//All private timers are always clocked at 1/2 of the CPU frequency (CPU_3x2x).
	*/
	
};

// Timer used by Alarm

typedef IF<zynq, Zynq_Timer, PandaBoard_Timer>::Result Base;

class Alarm_Timer: public Base
{
public:
    static const unsigned int FREQUENCY = 1000;

public:
    Alarm_Timer(const Handler * handler):
    Base(FREQUENCY, handler, ALARM) {}
};


// Timer used by Thread::Scheduler
class Scheduler_Timer: public Base
{
private:
    typedef unsigned long Microsecond;

public:
    Scheduler_Timer(const Microsecond & quantum, const Handler * handler): 
    Base(1000000 / quantum, handler, SCHEDULER) {}
};

// Timer used by ARM7_TSC
class TSC_Timer: public Base
{
public:
    TSC_Timer(const Hertz & freq, const Handler * handler) 
    : Base(1, handler, TSC) {}
};

__END_SYS

#endif

