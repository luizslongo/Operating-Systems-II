// EPOS-- MC13224V Interrupt Controller Mediator Declarations

#ifndef __mc13224v_ic_h
#define __mc13224v_ic_h

#include <ic.h>

#include <cpu.h>
#include <system/memory_map.h>

__BEGIN_SYS

class MC13224V_IC: public IC_Common
{
private:
	typedef IO_Map<Machine> IO;
	
protected:

	typedef void (*Interrupt_Handler)();
	static Interrupt_Handler vector[];


    MC13224V_IC() {}

public:
	
	enum {
		IRQ_SWI = 0,
		IRQ_UART1,
		IRQ_UART2,
		IRQ_CRM,
		IRQ_I2C,
		IRQ_TIMER,
		IRQ_SPIF,
		IRQ_MACA,
		IRQ_SSI,
		IRQ_ADC,
		IRQ_SPI,
        IRQ_KBI0,
        IRQ_KBI1,
        IRQ_KBI2,
        IRQ_KBI3,
        IRQ_KBI4,
        IRQ_KBI5,
        IRQ_KBI6,
        IRQ_KBI7,
    };

	static void enable(Interrupt_Id id) {
		CPU::out32(IO::ITC_INTENNUM, id);
	}

	static void disable() {
		CPU::out32(IO::ITC_INTENABLE, 0x00000000);
	}

	static void disable(Interrupt_Id id) { 
		CPU::out32(IO::ITC_INTDISNUM, id);
	}

	static void force_interrupt(Interrupt_Id id) {
		CPU::out32(IO::ITC_INTFRC, (1 << id));
	}

	static bool interrupt_pending(Interrupt_Id id) {
	    return (((CPU::in32(IO::ITC_NIPEND) & (1 << id)) >> id) == 1);
	}

	static void int_vector(Interrupt_Id id, Interrupt_Handler handler){
		vector[id] = handler;
	}

	static void int_handler(CPU::Reg32 pending){
		
		switch(pending){
			
			case 1<<IRQ_TIMER:
				vector[IRQ_TIMER]();
			break;

			case 1<<IRQ_UART1:
				vector[IRQ_UART1]();
			break;

			case 1<<IRQ_UART2:
				vector[IRQ_UART2]();
			break;

			case 1<<IRQ_MACA:
				vector[IRQ_MACA]();
			break;

			default:
			break;
		}
	}

};



__END_SYS

#endif
