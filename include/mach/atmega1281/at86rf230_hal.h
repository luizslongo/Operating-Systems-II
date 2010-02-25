// EPOS-- AT86RF230_HAL Mediador Declarations

#ifndef AT86RF230_HAL_H_
#define AT86RF230_HAL_H_

#include <ic.h>
#include <machine.h>

__BEGIN_SYS

class AT86RF230_HAL
{

public:

	/*Registers' addresses*/
	enum Register {

		TRX_STATUS_REG   = 0x01,
		TRX_STATE_REG    = 0x02,
		TRX_CTRL_0_REG   = 0x03,
		PHY_TX_PWR_REG   = 0x05,
		PHY_RSSI_REG     = 0x06,
		PHY_ED_LEVEL_REG = 0x07,
		PHY_CC_CCA_REG   = 0x08,
		CCA_THRES		 = 0x09,
		IRQ_MASK_REG     = 0x0E,
		IRQ_STATUS_REG   = 0x0F,
		VREG_CTRL_REG    = 0x10,
		BATMON_REG       = 0x11,
		XOSC_CTRL_REG    = 0x12,
		FTN_CTRL_REG     = 0x18,
		VERSION_NUM_REG  = 0x1D,
		MAN_ID_0_REG     = 0x1E,
		MAN_ID_1_REG     = 0x1F,
		SHORT_ADDR_0_REG = 0x20,
		SHORT_ADDR_1_REG = 0x21,
		PAN_ID_0_REG     = 0x22,
		PAN_ID_1_REG     = 0x23,
		IEEE_ADDR_0_REG  = 0x24,
		IEEE_ADDR_1_REG  = 0x25,
		IEEE_ADDR_2_REG  = 0x26,
		IEEE_ADDR_3_REG  = 0x27,
		IEEE_ADDR_4_REG  = 0x28,
		IEEE_ADDR_5_REG  = 0x29,
		IEEE_ADDR_6_REG  = 0x2A,
		IEEE_ADDR_7_REG  = 0x2B,
		XAH_CTRL_REG     = 0x2C,
		CSMA_SEED_0_REG  = 0x2D,
		CSMA_SEED_1_REG  = 0x2E,
	};

	/*Constants*/
	enum{
		MAX_FRAME_LENGTH = 127
	};

public:

	typedef struct{
		unsigned char *data;
		unsigned char frame_length;
		unsigned char lqi;
	} Frame;


public:

	static void init();
	static void hw_reset();
	static unsigned char readRegister(Register reg);
	static void writeRegister(Register reg, unsigned char value);
	static void readFrameBuffer(Frame *frame);
	static void writeFrameBuffer(Frame *frame);
	static void setRSTlow();
	static void setRSThigh();
	static void setSLP_TRlow();
	static void setSLP_TRhigh();
	static void initInterrupts();
	static void interruptEnable();
	static void interruptDisable();
	static void setInterruptHandler(IC::Interrupt_Handler handler);

};

__END_SYS

#endif /* AT86RF230_HAL_H_ */
