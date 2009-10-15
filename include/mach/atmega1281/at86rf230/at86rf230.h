/*
 * AT86RF230.h
 *
 *  Created on: 12/01/2009
 *      Author: tiago
 */

#ifndef AT86RF230_H_
#define AT86RF230_H_

#include <mach/atmega1281/at86rf230/at86rf230_hal.h>
#include <nic.h>

namespace System{

class AT86RF230 {

public:
	typedef unsigned int microseconds_t;

	typedef enum{
		SUCESS,
		BUSY,
		FAILED,
		FAILED_OPERATING_MODE,
		TIME_OUT,
	}result_t;

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

public:
	AT86RF230();

	result_t forceValidState();

	void intEnable();

	void intDisable();

	state_t getState();

	operating_mode_t getOperatingMode();

	result_t setOperatingMode();

	result_t send(unsigned char *buffer, int size);

	result_t rx_on();

	result_t rx_off();

	result_t receive(unsigned char *buffer, int &size);

	bool dataReceived();

	result_t CCA_measurement(bool &result, CCA_mode_t mode = ENERGY_ABOVE_THRESHOLD,
			unsigned char ED_threshold = 0);


private:

	static void intHandler(unsigned int);

	static operating_mode_t operatting_mode;

};

}

#endif /* AT86RF230_H_ */
