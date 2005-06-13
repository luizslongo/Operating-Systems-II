// EPOS-- PC UART Mediator

#ifndef __pc_uart_h
#define __pc_uart_h

#include <uart.h>
#include <cpu.h>

__BEGIN_SYS

// National Semiconductor INS8250 UART
class INS8250
{
private:
    typedef IA32::IO_Port IO_Port;
    typedef IA32::Reg8 Reg8;
    typedef IA32::Reg16 Reg16;

public:
    // Register Addresses (relative to base I/O port)
    typedef IA32::Reg8 Address;
    enum {
	THR = 0, // Transmit Holding Register		W,   DLAB = 0
	RBR = 0, // Receive Buffer Register 		R,   DLAB = 0
	DLL = 0, // Divisor Latch LSB			R/W, DLAB = 1
	DLH = 0, // Divisor Latch MSB			R/W, DLAB = 1
	IER = 1, // Interrupt Enable Register		R/W, DLAB = 0
	FCR = 2, // FIFO Control Register		W
	IIR = 2, // Interrupt Identification Register	R
	LCR = 3, // Line Control Register		R/W
	MCR = 4, // Modem Control Register		R/W
	LSR = 5, // Line Status Register		R/W
	MSR = 6, // Modem Status Register		R/W
	SCR = 7  // Scratch Register			R/W
    };

public:
    INS8250(IO_Port p): _port(p) {
	reg(IER, 0); // Disable all interrupts
    }
    INS8250(IO_Port p, unsigned int div, unsigned int dbits,
	    unsigned int par, unsigned int sbits) : _port(p) {
	reg(IER, 0); // Disable all interrupts
	config(div, dbits, par, sbits);
    }

    Reg8 reg(Address addr) { return IA32::in8(_port + addr); }
    void reg(Address addr, Reg8 value) { IA32::out8(_port + addr, value); }

    void dlab(bool f) { 
	reg(LCR, reg(LCR) & 0x7f | (f << 8));
    }

    Reg16 divisor() {
	dlab(true); Reg16 div = IA32::in16(_port); dlab(false);	return div;
    }
    void divisor(Reg16 div) {
	dlab(true); IA32::out16(_port, div); dlab(false);
    }

    void config(unsigned int div, unsigned int dbits, 
		unsigned int par, unsigned int sbits) {
	// Set data word length (5, 6, 7 or 8)	
	Reg8 lcr = dbits - 5;
	    
	// Set parity (0 [no], 1 [odd], 2 [even])
	if(par) {
	    lcr |= 1 << 3;
	    lcr |= (par - 1) << 4;
	}

	// Set stop-bits (1, 2 or 3 [1.5])
	lcr |= (sbits > 1) ? (1 << 2) : 0;

	reg(LCR, lcr);

	// Set clock divisor
	divisor(div);
	    
	// Set DTR, RTS and OUT2 of MCR
	reg(MCR, reg(MCR) | 0xb);
    }
	
    void config(unsigned int * div, unsigned int * dbits,
		unsigned int * par, unsigned int * sbits) {
	Reg8 lcr = reg(LCR);

	// Get data word length (LCR bits 0 and 1)
	*dbits = (lcr & 0x03) + 5;

	// Get parity (LCR bits 3 [enable] and 4 [odd/even])
	*par = (lcr & 0x08) ? ((lcr & 0x10) ? 2 : 1 ) : 0;

	// Get stop-bits  (LCR bit 2 [0 - >1, 1&D5 -> 1.5, 1&!D5 -> 2)
	*sbits = (lcr & 0x04) ? ((*dbits == 5) ? 3 : 2 ) : 1;

	// Get clock divisor
	*div = divisor();
    }

    void loopback(bool f) { reg(MCR, reg(MCR) | (f << 4)); }

    Reg8 get() { return reg(RBR); }
    void put(Reg8 c) { reg(THR, c); }

private:
    IO_Port _port;
};

// National Semiconductors NS16550AF (PC16550D) UART
// class NS16550AF: public INS8250 {}

class PC_UART: protected UART_Common
{
private:
    typedef Traits<PC_UART> Traits;
    static const Type_Id TYPE = Type<PC_UART>::TYPE;

    typedef IA32::IO_Port IO_Port;
    typedef IA32::Reg8 Reg8;
    typedef IA32::Reg16 Reg16;

    static const unsigned int CLOCK = Traits::CLOCK / 16;

    typedef INS8250 Chip;

public:
    PC_UART(unsigned int unit = 0) : _uart(_ports[unit]) {}
    PC_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
	    unsigned int stop_bits, unsigned int unit = 0) 
    : _uart(_ports[unit], CLOCK / baud, data_bits, parity, stop_bits) {}

    void config(unsigned int baud, unsigned int data_bits,
		unsigned int parity, unsigned int stop_bits) {
	_uart.config(CLOCK / baud, data_bits, parity, stop_bits);
    }

    void loopback(bool f) { _uart.loopback(f); }

    int get() { return _uart.get(); }
    void put(int c) { _uart.put(c); }

    unsigned int baud() { return CLOCK / _uart.divisor(); }
    void baud(unsigned int b) { _uart.divisor(CLOCK / b); }

    unsigned int data_bits() {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	return dbits;
    }
    void data_bits(unsigned int d) {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	_uart.config(div, d, sbits, par);
    }

    unsigned int stop_bits() {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	return sbits;
    }
    void stop_bits(unsigned int s) {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	_uart.config(div, dbits, s, par);
    }

    unsigned int parity() {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	return par;
    }
    void parity(unsigned int p) {
	unsigned int div, dbits, sbits, par;
	_uart.config(&div, &dbits, &sbits, &par);
	_uart.config(div, dbits, sbits, p);
    }

    static int init(System_Info * si);

private:
    Chip _uart;
    static const IO_Port _ports[];
};

typedef PC_UART UART;

__END_SYS

#endif
