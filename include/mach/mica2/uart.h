// EPOS-- Mica2_UART Declarations

#ifndef __mica2_uart_h
#define __mica2_uart_h

#include <uart.h>
#include <machine.h>

__BEGIN_SYS

class Mica2_UART: public UART_Common
{
private:
    typedef Traits<Mica2_UART> _Traits;
    static const Type_Id TYPE = Type<Mica2_UART>::TYPE;
    typedef IO_Map<Mica2> IO;
    typedef AVR8::Reg8 Reg8;

    enum {
	//UART0 IO Registers
        UDR0	= IO::UDR0,
        UCSR0A	= IO::UCSR0A,
        UCSR0B	= IO::UCSR0B,
        UCSR0C	= IO::UCSR0C,
        UBRR0L	= IO::UBRR0L,
        UBRR0H	= IO::UBRR0H,
        //UART1 IO Registers
        UDR1	= IO::UDR1,
        UCSR1A	= IO::UCSR1A,
        UCSR1B	= IO::UCSR1B,
        UCSR1C	= IO::UCSR1C,
        UBRR1L	= IO::UBRR1L,
        UBRR1H	= IO::UBRR1H
    };

    //UART IO Register Bit Offsets
    enum {
        //UCSRA
        RXC	= 7,
        TXC 	= 6,
        UDRE	= 5,
        FE	= 4,
        DOR	= 3,
        UPE	= 2,
        U2X	= 1,
        MPCM	= 0,
        //UCSRB
        RXCIE	= 7,
        TXCIE	= 6,
        UDRIE	= 5,
        RXEN	= 4,
        TXEN	= 3,
        UCS2Z	= 2,
        RXB8	= 1,
        TXB8	= 0,
        //UCSRC
        UMSEL	= 6,
        UPM1	= 5,
        UPM0	= 4,
        USBS	= 3,
        UCSZ1	= 2,
        UCSZ0	= 1,
        UCPOL	= 0,
    };

    static const int BASE_CLOCK = __SYS(Traits)<CPU>::CLOCK / 512;
  
public:  

    Mica2_UART(int unit = 0) : _unit(unit) { 
	// Default: 9600 8N1
	ubrrh(0);
	ubrrl(47);
	ucsra(0);
	ucsrc(1 << UCSZ1 | 1 << UCSZ0);
	ucsrb(1 << TXEN | 1 << RXEN);
    }

    Mica2_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
	       unsigned int stop_bits, unsigned int unit = 0) : _unit(unit) {
	config(baud,data_bits,parity,stop_bits);
	ucsrb(1 << TXEN | 1 << RXEN);
    }

    ~Mica2_UART(){ 
	ubrrhl(0);
	ucsrb(0);  
    }

    void config(unsigned int baud, unsigned int data_bits,
		unsigned int parity, unsigned int stop_bits) {
	ubrrhl((BASE_CLOCK / (baud>>5)) - 1);
	
	unsigned char cfg = ((data_bits - 5) << UCSZ0) | ((stop_bits - 1) << USBS);
	if (parity) cfg |= (parity + 1) << UPM0;
	ucsrc(cfg);

	ucsra(0);
    }

    void config(unsigned int * baud, unsigned int * data_bits,
		unsigned int * parity, unsigned int * stop_bits) {

	*baud = (BASE_CLOCK / (ubrrhl() + 1))<<5;

	unsigned char rc = ucsrc();

	*data_bits = ((rc >> UCSZ0) + 5) & 0x0F;
	*stop_bits = ((rc >> USBS) + 1) & 0x03;

	*parity = (rc >> USBS) & 0x03;
	if(*parity) *parity -= 1; 

    }

    Reg8 rxd() { return udr(); }
    void txd(Reg8 c) { udr(c); }

    void reset() { 
	unsigned int b, db, p, sb;
	config(&b, &db, &p, &sb);
	config(b, db, p, sb);
    }

    void loopback(bool flag) { }

    void int_enable(bool receive = true, bool send = true,
		    bool line = true, bool modem = true) {
	ucsrb( (1 << TXEN) | (1 << RXEN) | 
	       (receive << RXCIE) | (send << UDRIE) );
    }
    void int_disable() { ucsrb(1 << TXEN | 1 << RXEN); }

    bool rxd_full() { return (ucsra() & (1 << RXC)); }
    bool txd_empty() { return (ucsra() & (1 << UDRE)); }

    void dtr() { }
    void rts() { }
    bool cts() { return true; }
    bool dsr() { return true; }
    bool dcd() { return true; }
    bool ri()  { return true; }

    bool overrun_error() { return (ucsra() & (1 << RXC)) ; }
    bool parity_error()  { return (ucsra() & (1 << UPE)) ; }
    bool framing_error() { return (ucsra() & (1 << FE)) ; }

    char get() { while(!rxd_full()); return rxd(); }
    void put(char c) { while(!txd_empty()); txd(c); }

    static int init(System_Info *si); 
    
 
private:

    typedef AVR8::Reg8 Reg8;
    typedef AVR8::Reg16 Reg16;

    Reg8 udr(){ return AVR8::in8((_unit == 0) ? UDR0 : UDR1); }
    void udr(Reg8 value){ AVR8::out8(((_unit == 0) ? UDR0 : UDR1),value); }   
    Reg8 ucsra(){ return AVR8::in8((_unit == 0) ? UCSR0A : UCSR1A); }
    void ucsra(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0A : UCSR1A),value); } 
    Reg8 ucsrb(){ return AVR8::in8((_unit == 0) ? UCSR0B : UCSR1B); }
    void ucsrb(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0B : UCSR1B),value); } 
    Reg8 ucsrc(){ return AVR8::in8((_unit == 0) ? UCSR0C : UCSR1C); }
    void ucsrc(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0C : UCSR1C),value); } 
    Reg8 ubrrl(){ return AVR8::in8((_unit == 0) ? UBRR0L : UBRR1L); }
    void ubrrl(Reg8 value){ AVR8::out8(((_unit == 0) ? UBRR0L : UBRR1L),value); } 
    Reg8 ubrrh(){ return AVR8::in8((_unit == 0) ? UBRR0H : UBRR1H); }
    void ubrrh(Reg8 value){ AVR8::out8(((_unit == 0) ? UBRR0H : UBRR1H),value); } 

    Reg16 ubrrhl() { 
	Reg16 value = ubrrl();
	value |= ((Reg16)ubrrh())<<8;
	return value;
    }

    void ubrrhl(Reg16 value) { 
	ubrrh((Reg8)(value>>8)); 
	ubrrl((Reg8)value); 
    }
    
private:

   int _unit;

};


typedef Mica2_UART UART;

__END_SYS

#endif
