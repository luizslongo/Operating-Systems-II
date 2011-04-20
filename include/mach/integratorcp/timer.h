#ifndef __integrator_timer_h
#define __integrator_timer_h

#include <timer.h>
#include <ic.h>
#include <machine.h>

__BEGIN_SYS

template<unsigned int UNIT>
class IntegratorTimer2IRQ { };

template<>
class IntegratorTimer2IRQ<0> {
    public: enum { value = IC::TIMERINT0 };
};
template<>
class IntegratorTimer2IRQ<1> {
    public: enum { value = IC::TIMERINT1 };
};
template<>
class IntegratorTimer2IRQ<2> {
    public: enum { value = IC::TIMERINT2 };
};

template<unsigned int UNIT=0>
class IntegratorCP_Timer: public Timer_Common
{
protected:
    static const unsigned long TIMER_LOAD    = 0x13000000L + (UNIT * 0x100);            
    static const unsigned long TIMER_VALUE   = 0x13000004L + (UNIT * 0x100);
    static const unsigned long TIMER_CONTROL = 0x13000008L + (UNIT * 0x100);
    
    static const unsigned char BIT_ENABLE = 7;
    static const unsigned char BIT_MODE   = 6; // 0 = free, 1 = periodic
    static const unsigned char BIT_IE     = 5; // interrupt enable
    static const unsigned char BIT_R      = 4; // unused
    static const unsigned char BIT_PRESCALE1 = 3;
    static const unsigned char BIT_PRESCALE0 = 2;
    static const unsigned char BIT_SIZE    = 1; // 0 = 16bit, 1 = 32bit
    static const unsigned char BIT_ONESHOT = 0; // 1 = oneshot
    
    
public:
    typedef void (*Handler)();
    
    static const unsigned long FREQUENCY = 24000000; // this should be checked

    IntegratorCP_Timer(Handler handler)
	{
        db<Timer>(TRC) << "Timer(unit="<<UNIT<<",hdl="<<(void*)handler<<")\n";
        set_handler(handler);
        enable();
    }
    IntegratorCP_Timer(unsigned long f,Handler handler) 
	{
        db<Timer>(TRC) << "Timer(unit="<<UNIT<<",hdl="<<(void*)handler<<",freq="<<f<<")\n";
        set_handler(handler);
        frequency(f);
        enable();
    }

    void set_handler(Handler handler) {
        IC::Interrupt_Id intr = IntegratorTimer2IRQ<UNIT>::value; 
        IC::enable(intr);
        IC::int_vector(intr, handler);   
    }

    Hertz frequency() const { 
		return CPU::in32(TIMER_LOAD) * FREQUENCY;
	}
    
    void frequency(const Hertz & f) {
        db<IC>(TRC) << "Timer<"<<UNIT<<">::frequency(f="<<f<<")\n";
        CPU::out32(TIMER_LOAD, f / FREQUENCY);
        CPU::Reg32 control = CPU::in32(TIMER_CONTROL);
        
        control |= 1L << BIT_MODE; // set periodic
        control |= 1L << BIT_IE;   // enable interrupt
        control |= 1L << BIT_SIZE; // 32bit counter
        
        CPU::out32(TIMER_CONTROL, control);
    };

    static void init() {}

    void reset() {
        db<IC>(TRC) << "Timer<"<<UNIT<<">::reset()\n";
        CPU::out32(TIMER_VALUE,CPU::in32(TIMER_LOAD));
	}
    
    void enable(){
        db<IC>(TRC) << "Timer<"<<UNIT<<">::enable()\n";
        CPU::Reg32 control = CPU::in32(TIMER_CONTROL) | (1L << BIT_IE);
        CPU::out32(TIMER_CONTROL, control);
	}

    void disable() {
        db<IC>(TRC) << "Timer<"<<UNIT<<">::disable()\n";
        CPU::Reg32 control = CPU::in32(TIMER_CONTROL) & ~(1L << BIT_IE);
        CPU::out32(TIMER_CONTROL, control);
	}

    Tick read() {
		return CPU::in32(TIMER_VALUE);
	}

};

__END_SYS

#endif

