// EPOS-- AVR8 Declarations 

#ifndef __avr8_h
#define __avr8_h

#include <cpu.h>
#include <utility/debug.h>
#include <system/info.h>

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
    FLAG_CARRY        = 0x01,
    FLAG_ZERO         = 0x02,
    FLAG_NEGATIVE     = 0x04,
    FLAG_TWO_COMPL_OF = 0x08,
    FLAG_SIGN         = 0x10,   
    FLAG_HALFCARRY    = 0x20,
    FLAG_BIT_COPY_ST  = 0x40,
    FLAG_INTERRUPT    = 0x80,
    FLAG_DEFAULTS     = FLAG_INTERRUPT, 
    FLAG_CLEAR        = ~FLAG_DEFAULTS
  };

  // CPU Context
  class Context {
		
  public:
    Context(Log_Addr entry) :_sreg(FLAG_DEFAULTS), _pc(AVR8::swap_bytes(entry)){}   
    Context() {}

    void save() volatile;
    void load() volatile;

    friend Debug & operator << (Debug & db, const Context & c) {
	db << "{"
	   << "sp="  << &c
	   << ",sreg=" << c._sreg
	   << "}" ;
	return db;
    }

  public:
      Reg8 _sreg;
      Reg8 _r0;
//    Reg8 _r1;		// r1 is always 0, thus not part of ctx
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

  // I/O ports
  typedef volatile Reg8 IO_Port;
  typedef volatile Reg8 IO_Irq;    
  struct IO_Ports {
    IO_Port twbr;    	// [0x20]
    IO_Port twsr;    	// [0x21]
    IO_Port twar;    	// [0x22]
    IO_Port twdr;    	// [0x23]
    IO_Port adcl;    	// [0x24]
    IO_Port adch;    	// [0x25]
    IO_Port adcsra;     // [0x26]
    IO_Port admux;    	// [0x27]
    IO_Port acsr;    	// [0x28]   
    IO_Port ubrrl;    	// [0x29]
    IO_Port ucsrb;    	// [0x2A]
    IO_Port ucsra;    	// [0x2B]
    IO_Port udr;    	// [0x2C]
    IO_Port spcr;    	// [0x2D]
    IO_Port spsr;    	// [0x2E]
    IO_Port spdr;    	// [0x2F]
    IO_Port pind;    	// [0x30]
    IO_Port ddrd;    	// [0x31]
    IO_Port portd;    	// [0x32]
    IO_Port pinc;    	// [0x33]
    IO_Port ddrc;    	// [0x34]
    IO_Port portc;    	// [0x35]
    IO_Port pinb;    	// [0x36]
    IO_Port ddrb;    	// [0x37]
    IO_Port portb;    	// [0x38]
    IO_Port pina;    	// [0x39] 
    IO_Port ddra;    	// [0x3A]
    IO_Port porta;    	// [0x3B] 
    IO_Port eecr;    	// [0x3C] 
    IO_Port eedr;    	// [0x3D]
    IO_Port eearl;    	// [0x3E]
    IO_Port eearh;    	// [0x3F]
    union
    {
      IO_Port ubrrh;	// [0x40]
      IO_Port ucsrc;
    };
    IO_Port wdtcr;    	// [0x41]
    IO_Port assr;    	// [0x42]
    IO_Port ocr2;    	// [0x43]
    IO_Port tcnt2;    	// [0x44]
    IO_Port tccr2;    	// [0x45]
    IO_Port icr1l;    	// [0x46]
    IO_Port icr1h;    	// [0x47]
    IO_Port ocr1bl;     // [0x48]
    IO_Port ocr1bh;     // [0x49]
    IO_Port ocr1al;     // [0x4A]
    IO_Port ocr1ah;     // [0x4B]
    IO_Port tcnt1l;     // [0x4C]
    IO_Port tcnt1h;     // [0x4D]
    IO_Port tccr1b;     // [0x4E]
    IO_Port tccr1a;     // [0x4F]
    IO_Port sfior;    	// [0x50]
    union
    {
      IO_Port osccal;	// [0x51]
      IO_Port ocdr;
    };
    IO_Port tcnt0;    	// [0x52]
    IO_Port tccr0;    	// [0x53]
    IO_Port mcucsr;     // [0x54]
    IO_Port mcucr;    	// [0x55]
    IO_Port twcr;    	// [0x56]
    IO_Port spmcr;    	// [0x57]
    IO_Port tifr;    	// [0x58]
    IO_Port timsk;    	// [0x59]
    IO_Port gifr;    	// [0x5A]
    IO_Port gicr;     	// [0x5B]
    IO_Port ocr0;    	// [0x5C] 
    IO_Port spl;    	// [0x5D]
    IO_Port sph;    	// [0x5E]
    IO_Port sreg;    	// [0x5F]
  };
	

 public:
  AVR8() {} 
  ~AVR8() {}
 
  Hertz clock(){  return Traits::CLOCK; };

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

  static Reg32 htonl(Reg32 v)	{
    return htonl_lsb(v);
  }
  
  static Reg16 htons(Reg16 v)	{
    return htons_lsb(v);
  }
  
  static Reg32 ntohl(Reg32 v)	{
    return htonl(v);
  }
  
  static Reg16 ntohs(Reg16 v)	{
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
    ASMV("mov 	%A0,r24"	"\n"
    	 "mov 	%B0,r25"	"\n"
	 : "=r" (value)
	 : 
    );
    return value;
  }  
  
  static void r25_24(Reg16 value){
    ASMV("mov 	r24,%A0"	"\n"
    	 "mov 	r25,%B0"	"\n"
	 : 
	 : "r" (value)
    );  
  }
  
  static Log_Addr pc(){
    Log_Addr value;
    ASMV("cli"			"\n"
    	 "call 1f"		"\n"
    	 "1:"			"\n"
	 "in 	%A0,0x3e"	"\n"
    	 "in 	%B0,0x3d"	"\n"
	 "pop 	r1"		"\n"
	 "pop	r1"		"\n"
	 "clr	r1"		"\n"
	 "sei"			"\n"
	 : "=r" (value)
	 : 
	);
    return value;  
  }
    
  static Reg8 in8(const unsigned char port) {
      Reg8 value;
      ASMV("in	%0,%1"		"\n"
	   : "=r" (value)
	   : "I"  (port)
      );
      return value;
  }
  
  static Reg16 in16(const unsigned char port) {
      Reg16 value;
      ASMV("in 	%A0,	%1"	"\n" // Must read low byte first
      	   "in 	%B0,	(%1)+1"	"\n" 
	   : "=r" (value)
	   : "I"   (port)
      );
      return value;
  }  
    
  static void out8(unsigned char port, Reg8 value) {
    (*(volatile unsigned char *)(port + 0x20)) = value;
  }
  
  static void out16(unsigned char port, Reg16 value) {
      ASMV("in 	(%0)+1,	%B1"	"\n" // Must write high byte first
           "in 	%0,	%A1"	"\n"
	   : "=I" (port)
	   : "r" (value)
      );
  }  
  
  static int init(System_Info * si);

  // This will be eliminated
  static IO_Ports * io;
    
 private:

  // This will be replaced
  static unsigned int swap_bytes(unsigned int i)
  {
    return ((i << 8) | (i >> 8));
  }
};

__END_SYS

#endif
