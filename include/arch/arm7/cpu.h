// EPOS-- ARM7 CPU Mediator Declarations

#ifndef __arm7_h
#define __arm7_h

#include <cpu.h>

__BEGIN_SYS

class ARM7: public CPU_Common
{
private:
    typedef Traits<CPU> _Traits;
    static const unsigned int CLOCK = 0;//Traits<Machine>::CLOCK;

public:
    // CPU Flags
    typedef Reg32 Flags;

    enum {
    FULL                = 0,
    IDLE                = 1,
    ADC_NOISE_REDUCTION = 2,
    POWER_DOWN          = 3,
    POWER_SAVE          = 4,
    NATIVE_STANDBY      = 5, //For this mode an external oscilator is needed
    EXTENDED_STANDBY    = 6, //For this mode an external oscilator is needed
    LIGHT               = IDLE,
    STANDBY             = POWER_SAVE,
    OFF                 = POWER_DOWN
    };

    // CPU Context
    class Context
    {
    public:
        Context(Log_Addr entry, Log_Addr stack_bottom, Log_Addr exit) :
			_r0(0), _r1(0), _r2(0), _r3(0), _r4(0), _r5(0), _r6(0),
			_r7(0), _r8(0), _r9(0), _r10(0), _r11(0), _r12(0),
            _sp(stack_bottom), _lr(exit), _pc(entry),
			_cpsr(0x60000013) {} // all interrupts enabled by defaul

        Context() {}

        void save() volatile;
        void load() const volatile;

        friend Debug & operator << (Debug & db, const Context & c) {
        db << "{sp="    << &c
           << "}" ;
        return db;
        }

    public:
        Reg32 _r0;
        Reg32 _r1;
        Reg32 _r2;
        Reg32 _r3;
        Reg32 _r4;
        Reg32 _r5;
        Reg32 _r6;
        Reg32 _r7;
        Reg32 _r8;
        Reg32 _r9;
        Reg32 _r10;
        Reg32 _r11;
        Reg32 _r12;
		Reg32 _sp;
        Reg32 _lr;
		Reg32 _pc;
        Reg32 _cpsr; // Current Program Status Register  
    };

public:
    ARM7() {}

    static void init();

    static void int_enable() {
		irq_enable();
		fiq_enable();
	}

    static void int_disable() {
		irq_disable();
		fiq_disable();
	}

    static void irq_enable() {
		ASMV("	mrs r1, cpsr\n"
			 "	bic r1, r1, #0x80\n"
			 "	msr cpsr_c, r1\n");
	}

    static void irq_disable() {
		ASMV("	mrs r1, cpsr\n"
			 "	orr r1, r1, #0x80\n"
			 "	msr cpsr_c, r1\n");
	}

    static void fiq_enable() {}
    static void fiq_disable() {}

    static void halt() {
		int_enable();
		for(;;); // is there a halt instruction?
	}

    static void switch_context(Context * volatile * o, Context * volatile n) __attribute__((naked));

    static Flags flags() {  return 0; } // return cprs(); }
    static void flags(Flags flags) { }  //sreg(flags); }

    static Reg32 sp() {     return 0; } // return sphl(); }
    static void sp(Reg32 sp) {} // sphl(sp); }

    static Reg32 fr() {
		Reg32 return_value;
		ASMV("	mov %0, r0\n"
				: "=r" (return_value)
				:
				:);
		return return_value;
	}

    static void fr(Reg32 fr) {	
		ASMV("	mov r0, %0\n"
				: 
				:"r" (fr)
				:);
	} 

    static Reg32 pdp() {    return 0;} // return 0; }
    static void pdp(Reg32 pdp) {}
    
    static Log_Addr ip() {  return 0;} // return pc(); }    

    static bool tsl(volatile bool & lock) {
        int_disable();
        register bool old = CPU_Common::tsl(lock);
        int_enable();
        return old;
    }
    static int finc(volatile int & value) {
        int_disable();
        register int old = CPU_Common::finc(value);
        int_enable();
        return old; 
    
    }
    static int fdec(volatile int & value) {
        int_disable();
        register int old = CPU_Common::fdec(value);
        int_enable();
        return old;
    }

    static Reg32 htonl(Reg32 v) { return swap32(v); }
    static Reg16 htons(Reg16 v) { return swap16(v); }
    static Reg32 ntohl(Reg32 v) { return htonl(v); }
    static Reg16 ntohs(Reg16 v) { return htons(v); }

    static Context * init_stack(
    Log_Addr stack, unsigned int size,
    void (* exit)(), int (* entry)()) {
    	Log_Addr sp = stack + size;
		sp -= sizeof(Context); //stack bottom
		return new(sp) Context(entry, sp - sizeof(unsigned int), Log_Addr(exit));
	}

    template<typename T1>
    static Context * init_stack(
    Log_Addr stack, unsigned int size,
    void (* exit)(), int (* entry)(T1 a1), T1 a1) {
    return 0;
    }

    template<typename T1, typename T2>
    static Context * init_stack(
    Log_Addr stack, unsigned int size, void (* exit)(),
    int (* entry)(T1 a1, T2 a2), T1 a1, T2 a2) {
    return 0;
    }


    template<typename T1, typename T2, typename T3>
    static Context * init_stack(
    Log_Addr stack, unsigned int size, void (* exit)(),
    int (* entry)(T1 a1, T2 a2, T3 a3), T1 a1, T2 a2, T3 a3) {
    return 0;
    }

	// ARM7 specific methods

	static Reg8 in8(const Reg32 port) {
		return (*(volatile Reg8 *)port);
	}

	static Reg16 in16(const Reg32 port) {
		return (*(volatile Reg16 *)port);
	}

	static Reg32 in32(const Reg32 port) {
		return (*(volatile Reg32 *)port);
	}

	static void out8(const Reg32 port, const Reg8 value) {
		(*(volatile Reg8 *)port) = value;
	}

	static void out16(const Reg32 port, const Reg16 value) {
		(*(volatile Reg16 *)port) = value;
	}  

	static void out32(const Reg32 port, const Reg32 value) {
		(*(volatile Reg32 *)port) = value;
	}
};

__END_SYS

#endif
