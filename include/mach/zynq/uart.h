#ifndef __panda_uart_h__
#define __panda_uart_h__

#include <uart.h>
#define SIM_MODE
//#include "dumb_init.h"
__BEGIN_SYS
class Zynq_UART : public UART_Common
{
	//Reference: Zynq-7000 All Programmable SoC, Technical Reference Manual.
	//ug585-Zynq-7000-TRM.pdf
	typedef unsigned long ulong;
	volatile ulong* _base;
	int _debug;
    enum {
        UART0_BASE = 0xE0000000,
        UART1_BASE = 0xE0001000,
    };
	public:
    Zynq_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
    unsigned int stop_bits, unsigned int unit = 0){}

	Zynq_UART(unsigned int base = 1){
		_debug = 1;
		if(base == 0)
			_base = (volatile ulong*)UART0_BASE;
		else if(base == 1)
			_base = (volatile ulong*)UART1_BASE;
		else
			while(_debug); //ERROR!

		//(*(volatile unsigned int*)0xe0001020) = 0xf;
		//_debug = (*(volatile unsigned int*)0xe0001020);
		//dumb_init::dumb_init(2);
		init();
	}
	private:
    //registers offset
    enum {
        CONTROL_REG0            = 0x00,
        MODE_REG0               = 0x04,
        INTRPT_EN_REG0          = 0x08,
        INTRPT_DIS_REG0         = 0x0C,
        INTRPT_MASK_REG0        = 0x10,
        CHNL_INT_STS_REG0       = 0x14,
        BAUD_RATE_GEN_REG0      = 0x18,
		RCVR_TIMEOUT_REG0		= 0x1C,
        RCVR_FIFO_TRIGGER_LEVEL0= 0x20,
        MODEM_CTRL_REG0         = 0x24,
        MODEM_STS_REG0          = 0x28,
        CHANNEL_STS_REG0        = 0x2C,
        TX_RX_FIFO0             = 0x30,
        BAUD_RATE_DIVIDER_REG0  = 0x34,
        FLOW_DELAY_REG0         = 0x38,
        TX_FIFO_TRIGGER_LEVEL0  = 0x44,
	};
	// System Level Control Registers (slcr)
	class SLCR{
		public:
		enum{
			UART_RST_CTRL	= 0xF8000228,
			UART_CLK_CTRL	= 0xF8000154,
			SLCR_UNLOCK		= 0xF8000008,
			MIO_PIN_46		= 0xF80007B8,
			MIO_PIN_47		= 0xF80007BC,
		};
		//UART_RST_CTRL bits
		enum{
			UART1_REF_RST		= 1<<3,
			UART0_REF_RST		= 1<<2,
			UART1_CPU1X_RST		= 1<<1,
			UART0_CPU1X_RST		= 1,
		};
		//UART_CLK_CTRL bits
		enum{
			DIVISOR			= 0x3f00,
			SRCSEL			= 0x30,
			CLKACT1			= 1<<1,
			CLKACT0			= 1,
		};
		//SLCR_UNLOCK bits
		enum{
			UNLOCK_KEY		= 0xffff,
		};
	};
    //Control_reg0 bits
    enum{
        STOPBRK     = 1<<8,
        STARTBRK    = 1<<7,
        RSTTO       = 1<<6,
        TXDIS       = 1<<5,
        TXEN        = 1<<4,
        RXDIS       = 1<<3,
        RXEN        = 1<<2,
        TXRES       = 1<<1,
        RXRES       = 1,
    };

    //MODE_REG0 bits
    enum{
        CHMODE      = (1<<9)|(1<<8),
        NBSTOP      = (1<<7)|(1<<6),
        PAR         = (1<<5)|(1<<4)|(1<<3),
        CHRL        = (1<<2)|(1<<1),
        CLKS        = 1,
    };
    //Intrpt_en_reg0, Intrpt_dis_reg0, 
    //Intrpt_mask_reg0 and Chnl_int_sts_reg0 bits
    enum{
        TOVR        = 1<<12,
        TNFUL       = 1<<11,
        TTRIG       = 1<<10,
        DMSI        = 1<<9,
        TIMEOUT     = 1<<8,
        PARE        = 1<<7,
        FRAME       = 1<<6,
        ROVR        = 1<<5,
        TFUL        = 1<<4,
        TEMPTY      = 1<<3,
        RFUL        = 1<<2,
        REMPTY      = 1<<1,
        RTRIG       = 1,
    };
    //Baud_rate_gen_reg0 bits
    enum{
        CD          = 0xffff,
    };
    //Rcvr_timeout_reg0 bits
    enum{
        RTO         = 0xff,
    };
    //Rcvr_FIFO_trigger_level0 bits
    enum{
        RCVR_RTRIG  = 0x3f,
    };
    //Modem_ctrl_reg0 bits
    enum{
        FCM         = 1<<5,
        RTS         = 1<<1,
        DTR         = 1,
    };
    //Modem_sts_reg0 bis
    enum{
        FCMS        = 1<<8,
        DCD         = 1<<7,
		RI			= 1<<6,
		DSR			= 1<<5,
		CTS			= 1<<4,
		DDCD		= 1<<3,
		TERI		= 1<<2,
		DDSR		= 1<<1,
        //DTR        = 1, defined above
    };
    //Channel_sts_reg0 bis
	class STS{ //just a namespace due to name conflicts.
		public:
		enum{
			TNFUL       = 1<<14,
			TTRIG       = 1<<13,
			FDELT		= 1<<12,
			TACTIVE		= 1<<11,
			RACTIVE		= 1<<10,
			TFUL		= 1<<4,
			TEMPTY		= 1<<3,
			RFUL		= 1<<2,
			REMPTY		= 1<<1,
			RTRIG       = 1,
		};
	};
	//TX_RX_FIFO0 bits
	enum{
		FIFO		= 0xff,
	};
	//Baud_rate_divider_reg0 bits
	enum{
		BDIV		= 0xff,
	};
	//Flow_delay_reg0 bits
	enum{
		FDEL		= 0x3f,
	};
	//Tx_FIFO_trigger_level0 bits
	enum{
		TTRIG_TxFIFO= 0x3f,
	};



	//Auxiliary functions.
	//Writes in the memory location "base", replacing previous values.
	inline void write(ulong base, ulong mask, ulong value){
		int n=0;
		//Matching mask with the value.
		while((mask & (1<<n)) == 0)
			++n;
		*((ulong*)base) = mask & (value << n);
	}	   
	/*Writes to the specified register (using _base), matching the 
	 * mask to write the value to the corresponding area.
	 * It uses a read-modify strategy (non-destructive).*/
	inline void write_bit(ulong reg, ulong mask, ulong value){
		if(value==1)
			return set_bit(reg, mask);
		if(value==0)
			return clear_bit(reg, mask);
		int n=0;
		//Matching mask with the value.
		while((mask & (1<<n)) == 0)
			++n;
		ulong* p = (ulong*)(_base + reg/sizeof(ulong));
		*p = ((*p)&(~mask)) | (mask & (value << n));
	}
	inline ulong read_bit(ulong reg, ulong mask){
		int n=0;
		while((mask & (1<<n)) == 0)
			++n;
		return (*(_base + reg/sizeof(ulong)) & mask) << n;
	}
	inline void set_bit(ulong reg, ulong mask){
		*(_base + reg/sizeof(ulong)) |= mask;
	}
	inline void clear_bit(ulong reg, ulong mask){
		*(_base + reg/sizeof(ulong)) &= ~mask;
	}
	
	inline ulong get_value(ulong reg){
		return *((ulong*)reg);
	}
    void init(){
		//Unlocking SLCR registers (p. 1500)
		write(SLCR::SLCR_UNLOCK, SLCR::UNLOCK_KEY, 0xDF0D);

		//(Following instructions from page 554 of the manual.)
        //Reset Controller (using option 2, p. 558)
		if((ulong)_base == UART0_BASE){
			write(SLCR::UART_RST_CTRL, SLCR::UART0_REF_RST, 0);
			write(SLCR::UART_RST_CTRL, SLCR::UART0_CPU1X_RST, 0);
		}
		else if((ulong)_base == UART1_BASE){
			write(SLCR::UART_RST_CTRL, SLCR::UART1_REF_RST, 0);
			write(SLCR::UART_RST_CTRL, SLCR::UART1_CPU1X_RST, 0);
		}
		else
			while(_debug); //should not get here

        /*Configure I/O signal routing*/
		write(SLCR::MIO_PIN_46, 0xffff, 0x12E1);
		write(SLCR::MIO_PIN_47, 0xffff, 0x12E0);

        /*	Configure UART_Ref_Clk (assuming I/O PPL = 667mhz.
			Dividing it by 13 to get 50mhz), based on p. 628.*/
		write(SLCR::UART_CLK_CTRL, SLCR::DIVISOR, 13); //decimal
		write(SLCR::UART_CLK_CTRL, SLCR::SRCSEL, 0);

		if((ulong)_base == UART0_BASE){
			write(SLCR::UART_CLK_CTRL, SLCR::CLKACT0, 1);
			write(SLCR::UART_CLK_CTRL, SLCR::CLKACT1, 0);//idk why that (p.558)
		}
		else if((ulong)_base == UART1_BASE){
			write(SLCR::UART_CLK_CTRL, SLCR::CLKACT0, 0);
			write(SLCR::UART_CLK_CTRL, SLCR::CLKACT1, 1);
		}

        //Configure controller functions
#ifndef SIM_MODE //Workaround due to qemu bug.
		write_bit(MODE_REG0, 0xFF, 0x20);
#endif
		write_bit(CONTROL_REG0, RXEN, 0);
		write_bit(CONTROL_REG0, RXDIS, 1);
		write_bit(CONTROL_REG0, TXEN, 0);
		write_bit(CONTROL_REG0, TXDIS, 1);

		write_bit(BAUD_RATE_DIVIDER_REG0, CD, 13);
		write_bit(BAUD_RATE_DIVIDER_REG0, BDIV, 217);

		write_bit(CONTROL_REG0, TXRES, 1);
		write_bit(CONTROL_REG0, RXRES, 1);

		write_bit(CONTROL_REG0, RXEN, 1);
		write_bit(CONTROL_REG0, RXDIS, 0);
		write_bit(CONTROL_REG0, TXEN, 1);
		write_bit(CONTROL_REG0, TXDIS, 0);

		//Disabling FIFO interrupts (option b)
		write_bit(RCVR_FIFO_TRIGGER_LEVEL0, RCVR_RTRIG, 0);
		write_bit(RCVR_TIMEOUT_REG0, RTO, 0);

		write_bit(CONTROL_REG0, 0xFFF, 0x117);

        //Configure Interrupts
		write_bit(RCVR_FIFO_TRIGGER_LEVEL0, RCVR_RTRIG, 1); //Arbitrarily set

		write_bit(INTRPT_DIS_REG0, RTRIG, 1);
		write_bit(INTRPT_EN_REG0, RTRIG, 0);
		if(*(ulong*)INTRPT_MASK_REG0 != 0) //for debug
			; //Problem
		
        //Configure modem controls (optional) TODO
    }
	public:
    //Manage Transmit and receive data
	void put(unsigned char ch){
#ifndef SIM_MODE //Workaround due to qemu bug.
		while((get_value(CHANNEL_STS_REG0) & TFUL) != 0);
		write_bit(TX_RX_FIFO0, FIFO, (ulong)ch);
#endif
	}
	unsigned char get(){
		while((get_value(CHANNEL_STS_REG0) & RTRIG) != 1);
		return (*(unsigned char*)(TX_RX_FIFO0)) & FIFO;
	}



};




/*******************************************************/





class TL16C750_UART : public UART_Common 
{
public:
    enum {
        UART1_BASE = 0x4806A000,
        UART2_BASE = 0x4806C000,
        UART3_BASE = 0x48020000,
        UART4_BASE = 0x4806E000,
    };
    
    //registers offset
    enum {
        THR     = 0x00,
        DLL     = 0x00,
        DLH     = 0x04,
        IER     = 0x04,
        FCR     = 0x08,
        LCR     = 0x0C,
        LSR     = 0x14,
        MDR1    = 0x20,
        SYNC    = 0x54,
        SYSS    = 0x58,        
    };
    
    //IER bits
    enum {
        RHR_IT          = 1,
        THR_IT          = (1 << 1),
        LINE_STS_IT     = (1 << 2),
        MODEM_STS_IT    = (1 << 3),
        SLEEP_MODE      = (1 << 4),
        XOFF_IT         = (1 << 5),
        RTS_IT          = (1 << 6),
        CTS_IT          = (1 << 7),
        IER_ENABLE_ALL  = 0xFF,
        IER_DISABLE_ALL = 0x00,
    };
    
    //FCR bits
    enum {
        FIFO_EN         = 1,
        RX_FIFO_CLEAR   = (1 << 1),
        TX_FIFO_CLEAR   = (1 << 2),
        DMA_MODE        = (1 << 3),
        //TX_FIFO_TRIG
        //RX_FIFO_TRIG 
    };
    
    //LCR bits
    enum {
        LENGTH_5     = 0x00,
        LENGTH_6     = 0x01,
        LENGTH_7     = 0x02,
        LENGTH_8     = 0x03,
        NB_STOP      = 1 << 2,
        PARITY_EN    = 1 << 3,
        PARITY_TYPE1 = 1 << 4,
        PARITY_TYPE2 = 1 << 5,
        BREAK_EN     = 1 << 6,
        DIV_EN       = 1 << 7,
        CONFIG_A     = 0x80,
        CONFIG_B     = 0xBF,
    };
    
    //LSR bits
    enum {
        RX_FIFO_E   = 1,
        RX_OE       = (1 << 1),
        RX_PE       = (1 << 2),
        RX_FE       = (1 << 3),
        RX_BI       = (1 << 4),
        TX_FIFO_E   = (1 << 5),
        TX_SR_E     = (1 << 6),
        RX_FIFO_STS = (1 << 7),
    };    
    
    //MDR1 bits
    enum {
        MDR1_SIR_MODE   = 0x01,
        MDR1_16_MODE    = 0x02,
        MDR1_13_MODE    = 0x03,
        MDR1_MIR_MODE   = 0x04,
        MDR1_FIR_MODE   = 0x05,
        MDR1_CIR_MODE   = 0x06,
        MDR1_DISABLE    = 0x07,
        MDR1_IR_SLEEP   = (1 << 3),
        MDR1_SET_TXIR   = (1 << 4),
        MDR1_SCT        = (1 << 5),
        MDR1_SIP_MODE   = (1 << 6),
        MDR1_FRAME_MODE = (1 << 7),
    };
    
    //SYNC bits
    enum {
        AUTOIDLE        = 1,
        SOFTRESET       = (1 << 1),
        ENAWAKEUP       = (1 << 2),
        NOIDLE          = (0x01 << 3),
        SMARTIDLE       = (0x02 << 3),
        SMARTIDLEWK     = (0x03 << 3),
    };
    
    //SYSS bits
    enum {
        RESETDONE = 1,
    };
    
    //default clock and baud rate
    enum {
        CLOCK = 48 * 1000 * 1000,
        BAUD_RATE = 115200,
    };
    
protected:
    const static unsigned char TX_BUFFER_FULL  = (1 << 5);
    const static unsigned char RX_BUFFER_EMPTY = (1 << 4);

public:
    TL16C750_UART(unsigned long base) {
        _base = (unsigned long*)base;
        _flag = (unsigned long*)(base + LSR);
        
        //reset and init UART
        //reset();
        //init(CLOCK, BAUD_RATE);
    }

    void put(unsigned char c) {
		/*
        while (!(*_flag & TX_FIFO_E)) ;
        *(_base + THR) = c;
		*/
    }

    char get() {
        while (!(*_flag & RX_FIFO_E)) ;
        //*_flag |= RX_FIFO_E;
        return (char)(*_base & 0xFF);
    }
    
    void init(unsigned int clock, unsigned int baud_rate) {
        // disable UART
        disable();
        
        //enable access to uart_fcr uart_ier 
        *(_base + LCR) = 0x00;
        
        //configure FIFOs
        *(_base + FCR) = FIFO_EN; 
        
        //disable interrupts and sleep mode
        *(_base + IER) = IER_DISABLE_ALL;
        
        //enable access to DLH e DLL
        *(_base + LCR) = CONFIG_B;
        
        //load the new divisor UART_16X mode
        unsigned int divisor = (unsigned int) clock / (baud_rate << 4);
        *(_base + DLL) = divisor;
        *(_base + DLH) = (divisor >> 8);
        
        //configure protocol formatting
        //char lenght = 8, stop bits = 1, no parity, no break control bit, normal operating condition
        *(_base + LCR) = (LENGTH_8);
        
        //switch to UART mode
        enable();
    }
    
private:
    void disable() {
        *(_base + MDR1) = MDR1_DISABLE;
    }
    
    void enable() {
        *(_base + MDR1) = MDR1_16_MODE;
    }
    
    void reset() {
        *(_base + SYNC) |= SOFTRESET;
        while(!(*(_base + SYSS) & RESETDONE)) ; 
    }

protected:
    volatile unsigned long * _base;
    volatile unsigned long * _flag;
};

class PandaBoard_UART : public TL16C750_UART {
public:
    PandaBoard_UART(unsigned int unit=0) :
        TL16C750_UART(UART3_BASE) {}

    // Dummy contructor with configuration
    PandaBoard_UART(unsigned int baud, unsigned int data_bits, unsigned int parity,
    unsigned int stop_bits, unsigned int unit = 0) : TL16C750_UART(UART3_BASE) {}
};
__END_SYS

#endif
