// EPOS-- AVRMCU_UART Declarations

// This work is licensed under the Creative Commons 
// Attribution-NonCommercial-NoDerivs License. To view a copy of this license, 
// visit http://creativecommons.org/licenses/by-nc-nd/2.0/ or send a letter to 
// Creative Commons, 559 Nathan Abbott Way, Stanford, California 94305, USA.

#ifndef __avrmcu_uart_h
#define __avrmcu_uart_h

#include <uart.h>

__BEGIN_SYS

class AVRMCU_UART: public UART_Common
{
private:
    typedef Traits<AVRMCU_UART> _Traits;
    static const Type_Id TYPE = Type<AVRMCU_UART>::TYPE;

    //UART IO Registers
    enum {
        UDR	= 0x0c,
        UCSRA	= 0x0b,
        UCSRB	= 0x0a,
        UCSRC	= 0x20,
        UBRRL	= 0x09,
        UBRRH	= 0x20
    };

    //UART IO Register Bits
    enum {
        //UCSRA
        RCX	= 0x80,
        TXC 	= 0x40,
        UDRE	= 0x20,
        FE	= 0x10,
        DOR	= 0x08,
        PE	= 0x04,
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
        URSEL	= 0x80,
        UMSEL	= 0x40,
        UPM1	= 0x20,
        UPM0	= 0x10,
        USBS	= 0x08,
        UCSZ1	= 0x04,
        UCSZ0	= 0x02,
        UCPOL	= 0x01
  };
  
    /* The UCSRC register cannot be read in an atomic operation,
       therefore we must keep its current value in a variable */
    static unsigned char __ucsrc ; 
  
public:  

    AVRMCU_UART() { /* Default Initialization is up to init */ }

    AVRMCU_UART(Baud_Rate br, Data_Bits db, Parity p, Stop_Bits sb)  { 
        __ucsrc = 0;
        set_baud_rate(br);
        set_data_bits(db);
        set_parity(p);
        set_stop_bits(sb);
	start();
    }

    ~AVRMCU_UART(){ 
	stop();
    }
    
    int set_baud_rate(Baud_Rate baudrate){
        unsigned long br = (Traits<CPU>::CLOCK / 8 / baudrate) - 1;
	ubrrh(!URSEL & (br>>8));
	ubrrl(br);     
        return 1;
    }

    int set_data_bits(Data_Bits databits){
        if (databits & D5)	__ucsrc &= (!UCSZ1 & !UCSZ0);
        if (databits & D6)	__ucsrc =  ((__ucsrc & !UCSZ1) | UCSZ0);
        if (databits & D7)	__ucsrc =  ((__ucsrc & !UCSZ0) | UCSZ1);
        if (databits & D8)	__ucsrc |= (UCSZ1 | UCSZ0);
	ucsrc(URSEL|__ucsrc);
        return 1;    
    }

    int set_stop_bits(Stop_Bits stopbits){
        if (stopbits & ONE_HALF_STOP_BITS)	return -1; 	// unsupported
        if (stopbits & ONE_STOP_BIT)		__ucsrc &= !USBS;
        if (stopbits & TWO_STPO_BITS)		__ucsrc |= USBS;
        ucsrc(URSEL|__ucsrc);	
        return 1;
    }

    int set_parity(Parity parity){
        if (parity & NO_PARITY)		__ucsrc &= (!UPM1 & !UPM0);
        if (parity & EVEN_PARITY) 	__ucsrc =  ((__ucsrc & !UPM0) | UPM1);
        if (parity & ODD_PARITY)    	__ucsrc |= (UPM1 | UPM0);
        ucsrc(URSEL|__ucsrc);
        return 1;
    }

    int receive_byte(unsigned char *rec_char){
        if (data_ready() != 1)
        	return -1;
        *rec_char = udr();
        return 1;
    }

    int send_byte(unsigned char send_char){
        if (line_empty() != 1) 
	return -1;
	udr(send_char);
        return 1;
    }

    int line_empty(){
        if (ucsra() & UDRE)
            return 1;
        return -1;
    }

    int data_ready(){
        if (ucsra() & RCX)
            return 1;
        return -1;
    }
    
    static int init(System_Info *si); 

private:

    typedef AVR8::Reg8 Reg8;
    typedef AVR8::Reg16 Reg16;
    
    static void start() {
	ucsrb(TXEN|RXEN);  
    }
    
    static void stop() {
	ucsrb(!TXEN & !RXEN);      
    }

    static Reg8 udr(){
	return AVR8::in8(UDR);
    }
    
    static void udr(Reg8 value){
	AVR8::out8(UDR,value);
    }   
    
    static Reg8 ucsra(){
	return AVR8::in8(UCSRA);
    }
    
    static void ucsra(Reg8 value){
	AVR8::out8(UCSRA,value);
    }    
    
    static Reg8 ucsrb(){
	return AVR8::in8(UCSRB);
    }
    
    static void ucsrb(Reg8 value){
	AVR8::out8(UCSRB,value);
    }         

    static Reg8 ucsrc(){
	return AVR8::in8(UCSRC);
    }
    
    static void ucsrc(Reg8 value){
	AVR8::out8(UCSRC,value);
    }  
    
    // UBRRH and UBRRL are not contiguous
    static Reg8 ubrrl(){
	return AVR8::in8(UBRRL);
    }
    
    static void ubrrl(Reg8 value){
	AVR8::out8(UBRRL,value);
    }     
    
    static Reg8 ubrrh(){
	return AVR8::in8(UBRRH);
    }
    
    static void ubrrh(Reg8 value){
	AVR8::out8(UBRRH,value);
    }     
    
    
   

};

typedef AVRMCU_UART UART;

__END_SYS

#endif
