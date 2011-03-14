// EPOS-- MC13224V Memory Map

#ifndef __memory_map_mc13224v_h
#define __memory_map_mc13224v_h

#include <system/memory_map.h>

__BEGIN_SYS

template <>
struct Memory_Map<MC13224V>
{
    enum {
    	MEM_BASE =  0,
	MEM_SIZE =  96 * 1024
    };

    enum {
	    BASE =      0x00400000,
    	TOP =       0x00418000,
	    APP_LO =    0x00000000,
	    APP_CODE =  0x00000000,
	    APP_DATA =  0x00000000,
	    APP_HI =    0x00000000,
	    PHY_MEM =   0x00400000,
	    IO_MEM =    0x80000000,
	    APIC_MEM =  0x00000000,
	    SYS =       0x00000000,
	    IDT =       SYS + 0x00000000,
	    GDT =       SYS + 0x00000000,
	    SYS_PT =    SYS + 0x00000000,
	    SYS_PD =    SYS + 0x00000000,
	    SYS_INFO =  0x00416000,
	    SYS_CODE =  SYS + 0x00000000,
	    SYS_DATA =  SYS + 0x00000000,
	    SYS_STACK = SYS + 0x00000000
    };
};

template <class Imp> struct IO_Map;
template <> struct IO_Map<MC13224V>
{
	enum {
		GPIO_BASE		= 0x80000000,
		GPIO_FUNC_SEL0	= GPIO_BASE + 0x18,
		GPIO_FUNC_SEL1	= GPIO_BASE + 0x1C,
		GPIO_FUNC_SEL2 	= GPIO_BASE + 0x20,
		GPIO_PAD_DIR_SET0 = GPIO_BASE + 0x58,
		GPIO_PAD_DIR_SET1 = GPIO_BASE + 0x5c,

		UART1_BASE		= 0x80005000,
		UART1_UCON		= UART1_BASE + 0x00,
		UART1_USTAT		= UART1_BASE + 0x04,
		UART1_UDATA		= UART1_BASE + 0x08,
		UART1_URXCON	= UART1_BASE + 0x0C,
		UART1_UTXCON	= UART1_BASE + 0x10,
		UART1_UCTS		= UART1_BASE + 0x14,
		UART1_UBRCNT	= UART1_BASE + 0x18,
		UART2_BASE		= 0x8000B000,
		UART2_UCON		= UART2_BASE + 0x00,
		UART2_USTAT		= UART2_BASE + 0x04,
		UART2_UDATA		= UART2_BASE + 0x08,
		UART2_URXCON	= UART2_BASE + 0x0C,
		UART2_UTXCON	= UART2_BASE + 0x10,
		UART2_UCTS		= UART2_BASE + 0x14,
		UART2_UBRCNT	= UART2_BASE + 0x18,

		// These are all 16 bits registers.
		// They must be accessed with out16 or in16 methods!
		TIMER0_BASE		= 0x80007000,
		TIMER0_COMP1	= TIMER0_BASE,
		TIMER0_COMP2	= TIMER0_BASE + 0x02,
		TIMER0_CAPT		= TIMER0_BASE + 0x04,
		TIMER0_LOAD		= TIMER0_BASE + 0x06,
		TIMER0_HOLD		= TIMER0_BASE + 0x08,
		TIMER0_CNTR		= TIMER0_BASE + 0x0A,
		TIMER0_CTRL		= TIMER0_BASE + 0x0C,
		TIMER0_SCTRL	= TIMER0_BASE + 0x0E,
		TIMER0_CMPLD1	= TIMER0_BASE + 0x10,
		TIMER0_CMPLD2	= TIMER0_BASE + 0x12,
		TIMER0_CSCTRL	= TIMER0_BASE + 0x14,
		TIMER0_ENBL		= TIMER0_BASE + 0x1E,
		TIMER1_BASE		= 0x80007020,
		TIMER1_COMP1	= TIMER1_BASE,
		TIMER1_COMP2	= TIMER1_BASE + 0x02,
		TIMER1_CAPT		= TIMER1_BASE + 0x04,
		TIMER1_LOAD		= TIMER1_BASE + 0x06,
		TIMER1_HOLD		= TIMER1_BASE + 0x08,
		TIMER1_CNTR		= TIMER1_BASE + 0x0A,
		TIMER1_CTRL		= TIMER1_BASE + 0x0C,
		TIMER1_SCTRL	= TIMER1_BASE + 0x0E,
		TIMER1_CMPLD1	= TIMER1_BASE + 0x10,
		TIMER1_CMPLD2	= TIMER1_BASE + 0x12,
		TIMER1_CSCTRL	= TIMER1_BASE + 0x14,
		TIMER2_BASE		= 0x80007040,
		TIMER2_COMP1	= TIMER2_BASE,
		TIMER2_COMP2	= TIMER2_BASE + 0x02,
		TIMER2_CAPT		= TIMER2_BASE + 0x04,
		TIMER2_LOAD		= TIMER2_BASE + 0x06,
		TIMER2_HOLD		= TIMER2_BASE + 0x08,
		TIMER2_CNTR		= TIMER2_BASE + 0x0A,
		TIMER2_CTRL		= TIMER2_BASE + 0x0C,
		TIMER2_SCTRL	= TIMER2_BASE + 0x0E,
		TIMER2_CMPLD1	= TIMER2_BASE + 0x10,
		TIMER2_CMPLD2	= TIMER2_BASE + 0x12,
		TIMER2_CSCTRL	= TIMER2_BASE + 0x14,
		TIMER3_BASE		= 0x80007060,
		TIMER3_COMP1	= TIMER3_BASE,
		TIMER3_COMP2	= TIMER3_BASE + 0x02,
		TIMER3_CAPT		= TIMER3_BASE + 0x04,
		TIMER3_LOAD		= TIMER3_BASE + 0x06,
		TIMER3_HOLD		= TIMER3_BASE + 0x08,
		TIMER3_CNTR		= TIMER3_BASE + 0x0A,
		TIMER3_CTRL		= TIMER3_BASE + 0x0C,
		TIMER3_SCTRL	= TIMER3_BASE + 0x0E,
		TIMER3_CMPLD1	= TIMER3_BASE + 0x10,
		TIMER3_CMPLD2	= TIMER3_BASE + 0x12,
		TIMER3_CSCTRL	= TIMER3_BASE + 0x14,

		ITC_BASE		= 0x80020000,
		ITC_INTCNTL		= ITC_BASE,
		ITC_NIMASK		= ITC_BASE + 0x04,
		ITC_INTENNUM	= ITC_BASE + 0x08,
		ITC_INTDISNUM	= ITC_BASE + 0x0C,
		ITC_INTENABLE	= ITC_BASE + 0x10,
		ITC_INTTYPE		= ITC_BASE + 0x14,
		ITC_NIVECTOR	= ITC_BASE + 0x28,
		ITC_FIVECTOR	= ITC_BASE + 0x2C,
		ITC_INTSRC		= ITC_BASE + 0x30,
		ITC_INTFRC		= ITC_BASE + 0x34,
		ITC_NIPEND		= ITC_BASE + 0x38,
		ITC_FIPEND		= ITC_BASE + 0x3C,

		ADC_BASE		= 0x8000D000,
		ADC_COMP_0		= ADC_BASE + 0x00,
		ADC_COMP_1		= ADC_BASE + 0x02,
		ADC_COMP_2		= ADC_BASE + 0x04,
		ADC_COMP_3		= ADC_BASE + 0x06,
		ADC_COMP_4		= ADC_BASE + 0x08,
		ADC_COMP_5		= ADC_BASE + 0x0A,
		ADC_COMP_6		= ADC_BASE + 0x0C,
		ADC_COMP_7		= ADC_BASE + 0x0E,
		ADC_BAT_COMP_OVER	= ADC_BASE + 0x10,
		ADC_BAT_COMP_UNDER	= ADC_BASE + 0x12,
		ADC_SEQ_1		= ADC_BASE + 0x14,
		ADC_SEQ_2		= ADC_BASE + 0x16,
		ADC_CONTROL		= ADC_BASE + 0x18,
		ADC_TRIGGERS	= ADC_BASE + 0x1A,
		ADC_PRESCALE	= ADC_BASE + 0x1C,
		ADC_FIFO_READ	= ADC_BASE + 0x20,
		ADC_FIFO_CONTROL	= ADC_BASE + 0x22,
		ADC_FIFO_STATUS	= ADC_BASE + 0x24,
		ADC_1_SR_HIGH	= ADC_BASE + 0x30,
		ADC_1_SR_LOW	= ADC_BASE + 0x32,
		ADC_2_SR_HIGH	= ADC_BASE + 0x34,
		ADC_2_SR_LOW	= ADC_BASE + 0x36,
		ADC_ON_TIME		= ADC_BASE + 0x38,
		ADC_CONVERT_TIME	= ADC_BASE + 0x3A,
		ADC_CLOCK_DIVIDER	= ADC_BASE + 0x3C,
		ADC_OVERRIDE	= ADC_BASE + 0x40,
		ADC_IRQ			= ADC_BASE + 0x42,
		ADC_MODE		= ADC_BASE + 0x44,
		ADC_AD1_RESULT		= ADC_BASE + 0x46,
		ADC_AD2_RESULT		= ADC_BASE + 0x48,

		MACA_BASE	= 0x80004000,
		MACA_RESET 	= MACA_BASE + 0x04,
		MACA_RANDOM 	= MACA_BASE + 0x08,
		MACA_CONTROL 	= MACA_BASE + 0x0C,
		MACA_STATUS	= MACA_BASE + 0x10,
		MACA_FRMPND	= MACA_BASE + 0x14,
		MACA_MC1322x_ID	= MACA_BASE + 0x18,
		MACA_TMREN	= MACA_BASE + 0x40,
		MACA_TMRDIS	= MACA_BASE + 0x44,
		MACA_CLK 	= MACA_BASE + 0x48,
		MACA_STARTCLK 	= MACA_BASE + 0x4C,
		MACA_CPLCLK 	= MACA_BASE + 0x50,
		MACA_SFTCLK 	= MACA_BASE + 0x54,
		MACA_CLKOFFSET 	= MACA_BASE + 0x58,
		MACA_RELCLK	= MACA_BASE + 0x5C,
		MACA_CPLTIM	= MACA_BASE + 0x60,
		MACA_SLOTOFFSET	= MACA_BASE + 0x64,
		MACA_TIMESTAMP	= MACA_BASE + 0x68,
		MACA_DMARX	= MACA_BASE + 0x80,
		MACA_DMATX	= MACA_BASE + 0x84,
		MACA_DMAPOLL	= MACA_BASE + 0x88,
		MACA_TXLEN	= MACA_BASE + 0x8C,
		MACA_TXSEQNR	= MACA_BASE + 0x90,
		MACA_SETRXLVL	= MACA_BASE + 0x94,
		MACA_GETRXLVL	= MACA_BASE + 0x98,
		MACA_IRQ 	= MACA_BASE + 0xC0,
		MACA_CLRIRQ	= MACA_BASE + 0xC4,
		MACA_SETIRQ	= MACA_BASE + 0xC8,
		MACA_MASKIRQ	= MACA_BASE + 0xCC,
		MACA_MACPANID	= MACA_BASE + 0x100,
		MACA_MAC16ADDR	= MACA_BASE + 0x104,
		MACA_MAC64HI 	= MACA_BASE + 0x108,
		MACA_MAC64LO	= MACA_BASE + 0x10C,
		MACA_FLTREJ	= MACA_BASE + 0x110,
		MACA_CLKDIV 	= MACA_BASE + 0x114,
		MACA_WARMUP	= MACA_BASE + 0x118,
		MACA_PREAMBLE	= MACA_BASE + 0x11C,
		MACA_FRAMESYNC0	= MACA_BASE + 0x124,
		MACA_FRAMESYNC1	= MACA_BASE + 0x128,
		MACA_TXACKDELAY	= MACA_BASE + 0x140,
		MACA_RXACKDELAY	= MACA_BASE + 0x144,
		MACA_EOFDELAY	= MACA_BASE + 0x148,
		MACA_CCADELAY	= MACA_BASE + 0x14C,
		MACA_RXEND	= MACA_BASE + 0x150,
		MACA_TXCCADELAY	= MACA_BASE + 0x154,
		MACA_KEY3	= MACA_BASE + 0x158,
		MACA_KEY2	= MACA_BASE + 0x15C,
		MACA_KEY1	= MACA_BASE + 0x160,
		MACA_KEY0	= MACA_BASE + 0x164,
		MACA_OPTIONS	= MACA_BASE + 0x180,

		CRM_BASE	= 0x80003000,
		CRM_SYS_CNTL	= CRM_BASE + 0x00,
        CRM_STATUS      = CRM_BASE + 0x18,
        CRM_MOD_STATUS  = CRM_BASE + 0x1C,
		CRM_VREG_CNTL 	= CRM_BASE + 0x48
	};
	
	typedef volatile unsigned char IO_Port;  
	struct IO_Ports {
		//todo
	};
};

__END_SYS

#endif

