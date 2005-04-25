// EPOS-- AVR8 CPU Mediator Declarations 

#ifndef __avr8_h
#define __avr8_h

#include <cpu.h>
#include <utility/debug.h>

__BEGIN_SYS

class AVR8: public CPU_Common
{

private:
    typedef Traits<AVR8> Traits;
    static const Type_Id TYPE = Type<AVR8>::TYPE;

public:
    // CPU Flags
    typedef Reg8 Flags;
    enum {    
        FLAG_CARRY           = 0x01,
        FLAG_ZERO            = 0x02,
        FLAG_NEGATIVE        = 0x04,
        FLAG_TWO_COMPL_OF    = 0x08,
        FLAG_SIGN            = 0x10,     
        FLAG_HALFCARRY       = 0x20,
        FLAG_BIT_COPY_ST     = 0x40,
        FLAG_INTERRUPT       = 0x80,
        FLAG_DEFAULTS        = FLAG_INTERRUPT, 
        FLAG_CLEAR           = ~FLAG_DEFAULTS
    };

    // CPU Context
    class Context {
        
    public:
        Context(Log_Addr entry) :_sreg(FLAG_DEFAULTS),
                                 _pc((entry << 8) | (entry >> 8)){}     
        Context() {}

        void save() volatile;
        void load() volatile;

        friend Debug & operator << (Debug & db, const Context & c) {
        db << "{"
           << "sp="    << &c
           << ",sreg=" << c._sreg
           << "}" ;
        return db;
        }

    public:
        Reg8 _sreg;
        Reg8 _r0;
//      Reg8 _r1;        // r1 is always 0, thus not part of ctx
        Reg8 _r2;
        Reg8 _r3;
        Reg8 _r4;
        Reg8 _r5;
        Reg8 _r6;
        Reg8 _r7;
        Reg8 _r8;
        Reg8 _r9;
        Reg8 _r10;
        Reg8 _r11;
        Reg8 _r12;
        Reg8 _r13;
        Reg8 _r14;
        Reg8 _r15;
        Reg8 _r16;
        Reg8 _r17;
        Reg8 _r18;
        Reg8 _r19;
        Reg8 _r20;
        Reg8 _r21;
        Reg8 _r22;
        Reg8 _r23;
        Reg8 _r24;
        Reg8 _r25;
        Reg8 _r26;
        Reg8 _r27;
        Reg8 _r28;
        Reg8 _r29;
        Reg8 _r30;
        Reg8 _r31;
        Reg16 _pc;        
    };

public:
    AVR8() {} 
    ~AVR8() {}
 
    Hertz clock(){    return Traits::CLOCK; };

    static void int_enable(){ASMV("sei");};
    static void int_disable(){ASMV("cli");};
    static void halt(){ASMV("sleep");};

    static void switch_context(Context * volatile * current, 
                   Context * volatile next);

    static Flags flags() { return sreg(); }
    static void flags(Flags flags) { sreg(flags); }


    static Reg16 sp() { return sphl(); }
    static void sp(Reg16 sp) { sphl(sp); }


    static Reg16 fr() { return r25_24(); }
    static void fr(Reg16 fr) { r25_24(fr); }

    static Reg16 pdp() { return 0; }
    static void pdp(Reg16 pdp) { }
    
    static Log_Addr ip() { return pc(); }    

    static bool tsl(volatile bool & lock) {
        int_disable();
        register bool old = CPU_Common::tsl(lock);
        int_enable();
        return old;
    }
        
    static int finc(volatile int & value) {
        int_disable();
        register bool old = CPU_Common::finc(value);
        int_enable();
        return old;
    }
    
    static int fdec(volatile int & value) {
        int_disable();
        register bool old = CPU_Common::finc(value);
        int_enable();
        return old;
    }

    static Reg32 htonl(Reg32 v) {
        return htonl_lsb(v);
    }
    
    static Reg16 htons(Reg16 v) {
        return htons_lsb(v);
    }
    
    static Reg32 ntohl(Reg32 v) {
        return htonl(v);
    }
    
    static Reg16 ntohs(Reg16 v) {
        return htons(v);
    }

    // AVR8 specific methods
    
    static Reg16 sphl() {
        return in16(0x3d);
    }
    
    static void sphl(Reg16 value) {
        out16(0x3d,value);
    }    
    
    static Reg8 sreg(){
        return in8(0x3f);
    }
    
    static void sreg(Reg8 value){
        out8(0x3f,value);
    }
    
    static Reg16 r25_24(){
        Reg16 value;
        ASMV("mov     %A0,r24"    "\n"
             "mov     %B0,r25"    "\n"
             : "=r" (value)
             : 
        );
        return value;
    }    
    
    static void r25_24(Reg16 value){
        ASMV("mov     r24,%A0"    "\n"
             "mov     r25,%B0"    "\n"
             : 
             : "r" (value)
        );    
    }
    
    static Log_Addr pc(){
        Log_Addr value;
        ASMV("cli"                "\n"
             "call 1f"            "\n"
             "1:"                 "\n"
             "in     %A0,0x3e"    "\n"
             "in     %B0,0x3d"    "\n"
             "pop     r1"         "\n"
             "pop    r1"          "\n"
             "clr    r1"          "\n"
             "sei"                "\n"
             : "=r" (value)
             : 
        );
        return value;  
    }
    
    static Reg8 in8(const unsigned char port) {
        Reg8 value;
        ASMV("in    %0,%1"        "\n"
         : "=r" (value)
         : "I"  (port)
        );
        return value;
    }
  
    static Reg16 in16(const unsigned char port) {
        Reg16 value;
        ASMV("in     %A0,    %1"        "\n" // Must read low byte first
             "in     %B0,    (%1)+1"    "\n" 
             : "=r" (value)
             : "I"   (port)
        );
        return value;
    }  
    
    static void out8(unsigned char port, Reg8 value) {
        (*(volatile unsigned char *)(port + 0x20)) = value;
    }
  
    static void out16(const unsigned char port, Reg16 value) {
        ASMV("out     (%0)+1,    %B1"   "\n" // Must write high byte first
             "out     %0,    %A1"       "\n"
             : 
             : "I" (port), "r" (value)
        );
    }  
  
    static int init(System_Info * si);
    
private:

};

__END_SYS

#endif
