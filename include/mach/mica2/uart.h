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
    typedef IO_Map<Machine> IO;

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

    //UART IO Register Bits
    enum {
        //UCSRA
        RXC	= 0x80,
        TXC 	= 0x40,
        UDRE	= 0x20,
        FE	= 0x10,
        DOR	= 0x08,
        UPE	= 0x04,
        U2X	= 0x02,
        MPCM	= 0x01,
        //UCSRB
        RXCIE	= 0x80,
        TXCIE	= 0x40,
        UDRIE	= 0x20,
        RXEN	= 0x10,
        TXEN	= 0x08,
        UCS2Z	= 0x04,
        RXB8	= 0x02,
        TXB8	= 0x01,
        //UCSRC
        UMSEL	= 0x40,
        UPM1	= 0x20,
        UPM0	= 0x10,
        USBS	= 0x08,
        UCSZ1	= 0x04,
        UCSZ0	= 0x02,
        UCPOL	= 0x01
    };

    static const long BASE_CLOCK = __SYS(Traits)<CPU>::CLOCK / 16;
  
public:  

    Mica2_UART() { //(int unit = 0) : _unit(unit) { 
	//set_baud_rate(B9600);
	ubrrh(0);
	ubrrl(47);
	ucsra(0);
	ucsrc(UCSZ1 | UCSZ0); // 8N1
	ucsrb(TXEN | RXEN);


    }

    Mica2_UART(unsigned int br, unsigned int db, unsigned int p,unsigned int sb/*,int unit = 0*/) /*: _unit(unit)*/ {
        set_baud_rate(br);
        set_data_bits(db);
        set_parity(p);
        set_stop_bits(sb);
	ucsrb(TXEN | RXEN);
    }

    ~Mica2_UART(){ 
	ucsrb(~TXEN & ~RXEN);  
    }
    
   
    int set_baud_rate(unsigned int baudrate){
	ubrrhl((BASE_CLOCK / baudrate) - 1);
        return 1;
    }

    int set_data_bits(unsigned int databits){
	switch (databits) {
	case 5: ucsrc(ucsrc() & ~UCSZ1 & ~UCSZ0); break;
	case 6: ucsrc(ucsrc() & ~UCSZ1 |  UCSZ0); break;
	case 7: ucsrc(ucsrc() &  UCSZ1 | ~UCSZ0); break;
	case 8: ucsrc(ucsrc() |  UCSZ1 |  UCSZ0); break;
	}
        return 1;    
    }

    int set_stop_bits(unsigned int stopbits){
	switch(stopbits) {
	case 1:      ucsrc(ucsrc() & ~USBS); break;
	case 2:      ucsrc(ucsrc() |  USBS); break;
	}
	return 1;
    }

    int set_parity(unsigned int parity){
	switch(parity){
	case 0: ucsrc(ucsrc() & ~UPM1 & ~UPM0);
	case 1: ucsrc(ucsrc() |  UPM1 & ~UPM0);
	case 2: ucsrc(ucsrc() |  UPM1 |  UPM0);
	}
        return 1;
    }

    char get() {
        while(data_ready() != 1);
	return udr();
    }

    void put(char c) {
        while(line_empty() != 1);
	udr(c);
    }

/*     int receive_byte(unsigned char *rec_char){ */
/*         if (data_ready() != 1) */
/* 	    return -1; */
/*         *rec_char = udr(); */
/*         return 1; */
/*     } */

/*     int send_byte(unsigned char send_char){ */
/*         if (line_empty() != 1)  */
/* 	    return -1; */
/* 	udr(send_char); */
/*         return 1; */
/*     } */

    int line_empty(){
        if (!(ucsra() & UDRE)) return -1;
        return 1;
    }

    int data_ready(){
        if (ucsra() & RXC)
            return 1;
        return -1;
    }

    static int init(System_Info *si); 
    
 
private:

    typedef AVR8::Reg8 Reg8;
    typedef AVR8::Reg16 Reg16;
    


//     Reg8 udr(){ return AVR8::in8((_unit == 0) ? UDR0 : UDR1); }
//     void udr(Reg8 value){ AVR8::out8(((_unit == 0) ? UDR0 : UDR1),value); }   
//     Reg8 ucsra(){ return AVR8::in8((_unit == 0) ? UCSR0A : UCSR1A); }
//     void ucsra(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0A : UCSR1A),value); } 
//     Reg8 ucsrb(){ return AVR8::in8((_unit == 0) ? UCSR0B : UCSR1B); }
//     void ucsrb(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0B : UCSR1B),value); } 
//     Reg8 ucsrc(){ return AVR8::in8((_unit == 0) ? UCSR0C : UCSR1C); }
//     void ucsrc(Reg8 value){ AVR8::out8(((_unit == 0) ? UCSR0C : UCSR1C),value); } 
//     Reg8 ubrrl(){ return AVR8::in8((_unit == 0) ? UBRR0L : UBRR1L); }
//     void ubrrl(Reg8 value){ AVR8::out8(((_unit == 0) ? UBRR0L : UBRR1L),value); } 
//     Reg8 ubrrh(){ return AVR8::in8((_unit == 0) ? UBRR0H : UBRR1H); }
//     void ubrrh(Reg8 value){ AVR8::out8(((_unit == 0) ? UBRR0H : UBRR1H),value); } 


    static Reg8 udr(){ return AVR8::in8(UDR0); }
    static void udr(Reg8 value){ AVR8::out8(UDR0,value); }   
    static Reg8 ucsra(){ return AVR8::in8(UCSR0A); }
    static void ucsra(Reg8 value){ AVR8::out8(UCSR0A,value); } 
    static Reg8 ucsrb(){ return AVR8::in8(UCSR0B); }
    static void ucsrb(Reg8 value){ AVR8::out8(UCSR0B,value); } 
    static Reg8 ucsrc(){ return AVR8::in8(UCSR0C); }
    static void ucsrc(Reg8 value){ AVR8::out8(UCSR0C,value); } 
    static Reg8 ubrrl(){ return AVR8::in8(UBRR0L); }
    static void ubrrl(Reg8 value){ AVR8::out8(UBRR0L,value); } 
    static Reg8 ubrrh(){ return AVR8::in8(UBRR0H); }
    static void ubrrh(Reg8 value){ AVR8::out8(UBRR0H,value); } 

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
