// EPOS-- ATMega1281 Transceiver Mediator Declarations

#ifndef AT86RF230_H_
#define AT86RF230_H_

#include "at86rf230_hal.h"
#include <transceiver.h>

__BEGIN_SYS

class ATMega1281_Transceiver : public Transceiver_Common {

public:

	typedef enum{
		BASIC_OPERATING_MODE,
		EXTENDED_OPERATING_MODE
	}operating_mode_t;

	typedef enum
	{
		CCA_DONE_IN_PROGRESS	= 0x00,
		CCA_DONE_FINISHED 		= 0x01,

		CCA_STATUS_BUSY		= 0x00,
		CCA_STATUS_IDLE		= 0x01,

		TRX_STATUS_P_ON                = 0x00,
		TRX_STATUS_BUSY_RX             = 0x01,
		TRX_STATUS_BUSY_TX             = 0x02,
		TRX_STATUS_RX_ON               = 0x06,
		TRX_STATUS_TRX_OFF             = 0x08,
		TRX_STATUS_PLL_ON              = 0x09,
		TRX_STATUS_SLEEP               = 0x0F,
		TRX_STATUS_BUSY_RX_AACK        = 0x11,
		TRX_STATUS_BUSY_TX_ARET        = 0x12,
		TRX_STATUS_RX_AACK_ON          = 0x16,
		TRX_STATUS_TX_ARET_ON          = 0x19,
		TRX_STATUS_RX_ON_NOCLK         = 0x1C,
		TRX_STATUS_RX_AACK_ON_NOCLK    = 0x1D,
		TRX_STATUS_BUSY_RX_AACK_NOCLK  = 0x1E,
		TRX_STATUS_STATE_TRANSITION_IN_PROGRESS = 0x1F,
	}state_t;

	typedef enum{
		ENERGY_ABOVE_THRESHOLD						= 0x01,
		CARRIER_SENSE_ONLY							= 0x02,
		CARRIER_SENSE_WITH_ENERGY_ABOVE_THRESHOLD	= 0x03
	}CCA_mode_t;

	enum{
		MAX_FRAME_SIZE = 127//AT86RF230_HAL::MAX_FRAME_LENGTH //TODO find out why this does not work
	};

private:
	//MASKS
	enum RegisterMask{
		TRX_STATUS_MASK_TRX_STATUS	= 0x1F,
		TRX_STATUS_MASK_CCA_DONE	= 0x80,
		TRX_STATUS_MASK_CCA_STATUS	= 0x40,

		PHI_CC_CCA_MASK_CCA_MODE	= 0x60,
		PHI_CC_CCA_MASK_CCA_REQUEST	= 0x80,
	};

	enum TRX_STATE_REG {
		TRAC_STATUS_SUCCESS                = 0x00,
		TRAC_STATUS_CHANNEL_ACCESS_FAILURE = 0x03,
		TRAC_STATUS_NO_ACK                 = 0x05,

		TRX_CMD_NOP            = 0x00,
		TRX_CMD_TX_START       = 0x02,
		TRX_CMD_FORCE_TRX_OFF  = 0x03,
		TRX_CMD_RX_ON          = 0x06,
		TRX_CMD_TRX_OFF        = 0x08,
		TRX_CMD_PLL_ON         = 0x09,
		TRX_CMD_RX_AACK_ON     = 0x16,
		TRX_CMD_TX_ARET_ON     = 0x19,
	};

	//Interrupt status and masks
	enum Interrupt{
		IRQ_STATUS_BAT_LOW		= 0x80,
		IRQ_STATUS_TRX_UR		= 0x40,
		IRQ_STATUS_TRX_END		= 0x08,
		IRQ_STATUS_RX_START		= 0x04,
		IRQ_STATUS_PLL_UNLOCK	= 0x02,
		IRQ_STATUS_PLL_LOCK		= 0x01,

		IRQ_STATUS_ALL			= 0xCF,
		IRQ_STATUS_UNKNOWN		= 0x00,
	};


public:
	ATMega1281_Transceiver(){}

	result_t forceValidState();

	void hardware_reset();

	void intEnable();

	void intDisable();

	state_t getState();

	operating_mode_t getOperatingMode();

	result_t setOperatingMode();

	result_t send(unsigned char *buffer, int size);

	result_t rx_on();

	result_t rx_off();

	result_t receive(unsigned char *buffer, int &size);

	void set_event_handler(Transceiver_Common::event_handler* handler);

	Transceiver_Common::event_handler* get_event_handler();

	result_t CCA_measurement(bool &result, CCA_mode_t mode = ENERGY_ABOVE_THRESHOLD,
			unsigned char ED_threshold = 0);


private:

	static void intHandler();

	static operating_mode_t operatting_mode;

	static Transceiver_Common::event_handler* ev_handler;

public:
	static void init();

};

__END_SYS

#endif /* AT86RF230_H_ */
