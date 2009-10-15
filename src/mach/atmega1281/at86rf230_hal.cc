/*
 * at86rf230_hal.h
 *
 *  Created on: 21/01/2009
 *      Author: tiago
 */

#include <mach/common/at86rf230/at86rf230_hal.h>
#include <ic.h>
#include <machine.h>
#include <cpu.h>
#include <traits.h>
#include <alarm.h>

namespace System {

typedef IO_Map<Machine> IO;

/*
	Radio pins

Mega1281 AT86RF230
PA7 -> RST
PB0 -> SEL
PB4 -> SLP_TR
PD6 -> CLKM
PE5 -> IRQ
 */
enum {
	SS_PIN       = 0,
	SCK_PIN      = 1,
	MOSI_PIN     = 2,
	MISO_PIN     = 3,
	SLP_TR		 = 4,
	RST			 = 7,
};

/*
SPI Registers
*/
enum {
	// SPCR
	SPIE     = 7,
	SPE      = 6,
	DORD     = 5,
	MSTR     = 4,
	CPOL     = 3,
	CPHA     = 2,
	SPR1     = 1,
	SPR0     = 0,
	// SPSR
	SPIF     = 7,
	WCOL     = 6,
	SPI2X    = 0,
};
/*
Commands
*/
enum {
	REG_READ	= 0x80,
	REG_WRITE	= 0xC0,
	FB_READ		= 0x20,
	FB_WRITE	= 0x60,
};


void SPI_SS_high(){
	AVR8::out8(IO::PORTB, AVR8::in8(IO::PORTB) | (1<<SS_PIN));
}

void SPI_SS_low(){
	AVR8::out8(IO::PORTB, AVR8::in8(IO::PORTB) & ~(1<<SS_PIN));
}

unsigned char SPI_dummy = 0;

void SPI_transmit(unsigned char toSlave, unsigned char &fromSlave){
	AVR8::out8(IO::SPDR, toSlave);
	while(!(AVR8::in8(IO::SPSR) & (1<<SPIF)));
	fromSlave = AVR8::in8(IO::SPDR);
}

bool AT86RF230_init_ok = false;

void AT86RF230_HAL::init(){

	if(AT86RF230_init_ok)
		return;

	AT86RF230_init_ok = true;

	Alarm::delay(500000); //time to enter state P_ON

	/*IO Specific Initialization.*/
	AVR8::out8(IO::DDRB, AVR8::in8(IO::DDRB) | (1 << SLP_TR)); //Enable SLP_TR as output.
	AVR8::out8(IO::DDRA, AVR8::in8(IO::DDRA) | (1 << RST)); //Enable RST as output.

	/*SPI Specific Initialization.*/
	//Set SS, CLK and MOSI as output.
	AVR8::out8(IO::DDRB, AVR8::in8(IO::DDRB) |
			((1 << SS_PIN) | (1 << SCK_PIN) | (1 << MOSI_PIN)));
	//Set SS and CLK high
	AVR8::out8(IO::PORTB, AVR8::in8(IO::PORTB) |
			((1 << SS_PIN) | (1 << SCK_PIN)));
	AVR8::out8(IO::SPCR, (1<<SPE)|(1<<MSTR));//Enable SPI module and master operation.
	AVR8::out8(IO::SPSR, (1 << SPI2X)); //Enable doubled SPI speed in master mode.

	/*reset the radio*/
	setRSTlow();
	setSLP_TRlow();
	Alarm::delay(500000); //time to reset
	setRSThigh();

}

unsigned char AT86RF230_HAL::readRegister(AT86RF230_HAL::Register reg){

	unsigned char value;

	SPI_SS_low();
	SPI_transmit((REG_READ | reg), SPI_dummy);//send command
	SPI_transmit(SPI_dummy, value); //read value
	SPI_SS_high();

	return value;
}

void AT86RF230_HAL::writeRegister(Register reg, unsigned char value){
	SPI_SS_low();
	SPI_transmit((REG_WRITE | reg), SPI_dummy); //send command
	SPI_transmit(value, SPI_dummy); //send value
	SPI_SS_high();
}

void AT86RF230_HAL::readFrameBuffer(AT86RF230_HAL::Frame *frame){
	SPI_SS_low();

	SPI_transmit(FB_READ, SPI_dummy); //send command
	SPI_transmit(SPI_dummy, frame->frame_length); //read frame size

	//read frame
	for(int i = 0; i < frame->frame_length; ++i)
		SPI_transmit(SPI_dummy, frame->data[i]);

	SPI_transmit(SPI_dummy, frame->lqi); //read lqi value

	SPI_SS_high();
}

void AT86RF230_HAL::writeFrameBuffer(AT86RF230_HAL::Frame *frame){
	SPI_SS_low();

	SPI_transmit(FB_WRITE, SPI_dummy); //send command
	SPI_transmit(frame->frame_length, SPI_dummy); //send frame size

	//write frame
	for(int i = 0; i < frame->frame_length; ++i)
		SPI_transmit(frame->data[i], SPI_dummy);

	SPI_SS_high();
}

void AT86RF230_HAL::setRSTlow(){
	AVR8::out8(IO::PORTA, AVR8::in8(IO::PORTA) & ~(1 << RST));
}

void AT86RF230_HAL::setRSThigh(){
	AVR8::out8(IO::PORTA, AVR8::in8(IO::PORTA) | (1 << RST));
}

void AT86RF230_HAL::setSLP_TRlow(){
	AVR8::out8(IO::PORTB, AVR8::in8(IO::PORTB) & ~(1 << SLP_TR));
}

void AT86RF230_HAL::setSLP_TRhigh(){
	AVR8::out8(IO::PORTB, AVR8::in8(IO::PORTB) | (1 << SLP_TR));
}

/*
 * Interrupt handling
 */

typedef enum{
	IRQ_0  = 0, // ??
	IRQ_1,	// ??
	IRQ_2,	// ??
	IRQ_3,	// ??
	IRQ_4,	// ??
	IRQ_5,	// radio
	IRQ_6,	// button 1
	IRQ_7,  // button 2
} irqNumber_t;

// interrupt activation condition.
typedef enum
{
	IRQ_LOW_LEVEL,     // The low level generates an interrupt request.
	IRQ_ANY_EDGE,      // Any edge generates an interrupt request.
	IRQ_FALLING_EDGE,  // Falling edge generates an interrupt request.
	IRQ_RISING_EDGE    // Rising edge generates an interrupt request.
} irqMode_t;

void enable_external_int(irqNumber_t irqNumber, irqMode_t irqMode){
	// IRQ pin is input
	CPU::out8(IO::DDRE, CPU::in8(IO::DDRE) & ~(1 << irqNumber));
	CPU::out8(IO::PORTE, CPU::in8(IO::PORTE) | (1 << irqNumber));
	unsigned char ui8ShiftCount = (irqNumber - IRQ_4) << 1;
	// Clear previous settings of corresponding interrupt sense control
	CPU::out8(IO::EICRB, CPU::in8(IO::EICRB) & ~(3 << ui8ShiftCount));
	// Setup corresponding interrupt sence control
	CPU::out8(IO::EICRB, CPU::in8(IO::EICRB) | ((irqMode & 0x03) << ui8ShiftCount));
	// Clear the INTn interrupt flag
	CPU::out8(IO::EIFR, CPU::in8(IO::EIFR) & ~(1 << irqNumber));
}

bool AT86RF230_init_interrupts_ok = false;

void AT86RF230_HAL::initInterrupts(){

	if(AT86RF230_init_interrupts_ok)
		return;

	AT86RF230_init_interrupts_ok = true;

	enable_external_int(IRQ_5, IRQ_RISING_EDGE);
	interruptEnable();
}

void AT86RF230_HAL::interruptEnable(){
	AVR8::out8(IO::EIMSK, CPU::in8(IO::EIMSK) | (1 << IRQ_5));
}

void AT86RF230_HAL::interruptDisable(){
	AVR8::out8(IO::EIMSK, CPU::in8(IO::EIMSK) & ~(1 << IRQ_5));
}

void AT86RF230_HAL::setInterruptHandler(Machine::int_handler handler){
	IC::int_vector(IC::IRQ_IRQ5, handler);
}

}

