/*
 * at86rf230.cc
 *
 *  Created on: 27/01/2009
 *      Author: tiago
 */

#include <mach/atmega1281/at86rf230/at86rf230.h>
#include <mach/atmega1281/at86rf230/at86rf230_hal.h>
#include <semaphore.h>
#include <alarm.h>

namespace System{

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

//initialize static members
AT86RF230::operating_mode_t AT86RF230::operatting_mode = AT86RF230::BASIC_OPERATING_MODE;

//variables
//volatile bool pll_locked = false;

//volatile bool sendingData = false;

//volatile bool _dataReceived = false;

Semaphore int_trx_end(0);


AT86RF230::AT86RF230(){
	AT86RF230_HAL::init();
	AT86RF230_HAL::setInterruptHandler(&intHandler);
	AT86RF230_HAL::initInterrupts();
	forceValidState();
	//TODO check when extended mode is implemented
	operatting_mode = AT86RF230::BASIC_OPERATING_MODE;
}

AT86RF230::result_t AT86RF230::forceValidState(){
	AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);
	//TODO check when extended mode is implemented
	operatting_mode = AT86RF230::BASIC_OPERATING_MODE;
	if(getState() != TRX_STATUS_TRX_OFF)
		return FAILED;
	else
		return SUCESS;
}

void AT86RF230::intEnable(){
	AT86RF230_HAL::interruptEnable();

}

void AT86RF230::intDisable(){
	AT86RF230_HAL::interruptDisable();
}

AT86RF230::state_t AT86RF230::getState(){
	unsigned char value = AT86RF230_HAL::readRegister(AT86RF230_HAL::TRX_STATUS_REG);

	value &= TRX_STATUS_MASK_TRX_STATUS;

	return static_cast<AT86RF230::state_t>(value);
}

AT86RF230::operating_mode_t AT86RF230::getOperatingMode(){
	//TODO implement
	return BASIC_OPERATING_MODE;
}

AT86RF230::result_t AT86RF230::setOperatingMode(){
	//TODO implement
	return FAILED;
}


unsigned char ckp_count = 0;
void checkpoint(){
	ckp_count = (ckp_count > 7)?0:(ckp_count+1);

	CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
	CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (ckp_count << 5));
}

AT86RF230::result_t AT86RF230::send(unsigned char *buffer, int size){

	ckp_count = 0;

	checkpoint();

	if(getState() != TRX_STATUS_TRX_OFF)
		return BUSY;

	if(operatting_mode == AT86RF230::BASIC_OPERATING_MODE){

		//sendingData = true;

		AT86RF230_HAL::Frame frame;
		frame.data = buffer;
		frame.frame_length = size;

		//write date to the frame buffer
		AT86RF230_HAL::writeFrameBuffer(&frame);

		//go to PLL_ON state and wait for pll to lock
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_PLL_ON);

		/*while(true){
			mutex_pll.p();
			if(pll_locked){
				mutex_pll.v();
				break;
			}
			mutex_pll.v();
		}*/
		while(getState() != TRX_STATUS_PLL_ON);

		checkpoint();

		//starting TX

		AT86RF230_HAL::setSLP_TRhigh();
		AT86RF230_HAL::setSLP_TRlow();

		/*if(getState() != TRX_STATUS_BUSY_TX)
			return FAILED;*/
		Alarm::delay(1000);

		//should be back to PLL_ON state when TX end
		while(getState() != TRX_STATUS_PLL_ON);

		checkpoint();

		//return to TRX_OFF
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_TRX_OFF);

		checkpoint();
		if(getState() != TRX_STATUS_TRX_OFF){
			//sendingData = false;
			return FAILED;
		}

		//sendingData = false;
		return SUCESS;
	}
	else{ //extended operating mode
		//TODO implement
		return FAILED;
	}
}


AT86RF230::result_t AT86RF230::rx_on(){

	if(getState() != TRX_STATUS_TRX_OFF)
		return BUSY;

	//_dataReceived = false;

	ckp_count = 0;

	if(operatting_mode == AT86RF230::BASIC_OPERATING_MODE){

		checkpoint();

		//go to RX_ON and wait pll to lock
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_RX_ON);

		checkpoint();

		/*while(true){
					mutex_pll.p();
					if(pll_locked){
						mutex_pll.v();
						break;
					}
					mutex_pll.v();
				}*/
		//while(!pll_locked);
		while(getState() != TRX_STATUS_RX_ON);

		checkpoint();

		if(getState() != TRX_STATUS_RX_ON)
			return FAILED;

		//checkpoint();

		return SUCESS;
	}
	else{ //extended operating mode
		//TODO implement
		return FAILED;
	}
}

AT86RF230::result_t AT86RF230::rx_off(){

	state_t state = getState();

	if(state == TRX_STATUS_BUSY_RX)
		return BUSY;
	else{
		return forceValidState();
	}
}

AT86RF230::result_t AT86RF230::receive(unsigned char *buffer, int &size){

	if(getState() != TRX_STATUS_TRX_OFF)
		return BUSY;

	//if(!dataReceived()){
	//	size = 0;
	//	return SUCESS;
	//}
	//else{

		AT86RF230_HAL::Frame frame;
		frame.data = buffer;

		//read the received data
		AT86RF230_HAL::readFrameBuffer(&frame);

		size = frame.frame_length;

		//_dataReceived = false;

		return SUCESS;
	//}
}

bool AT86RF230::dataReceived(){
	//return _dataReceived;
	bool aux = AT86RF230_HAL::readRegister(AT86RF230_HAL::IRQ_STATUS_REG) & IRQ_STATUS_RX_START;
	Alarm::delay(100000);
	return aux;
}

//OStream os2;

AT86RF230::result_t AT86RF230::CCA_measurement(bool &result, CCA_mode_t mode, unsigned char ED_threshold){

	if(getState() != TRX_STATUS_TRX_OFF)
		return BUSY;

	//set the energy threshold
	//????

	//set cca mode and request
	unsigned char regVal = AT86RF230_HAL::readRegister(AT86RF230_HAL::PHY_CC_CCA_REG);

	//os2 << "PHY_CC_CCA: " << static_cast<unsigned int>(regVal) << "\n";

	regVal &= ~PHI_CC_CCA_MASK_CCA_MODE;
	regVal |= (mode << 5);
	regVal |= PHI_CC_CCA_MASK_CCA_REQUEST;

	//os2 << "PHY_CC_CCA: " << static_cast<unsigned int>(regVal) << "\n";

	//start the measurement
	result_t aux = rx_on();
	if(aux != SUCESS)
		return FAILED;

	AT86RF230_HAL::writeRegister(AT86RF230_HAL::PHY_CC_CCA_REG, regVal);


	//poll the radio to check if the measurement is finished
	while(true){
		regVal = AT86RF230_HAL::readRegister(AT86RF230_HAL::TRX_STATUS_REG);
	//	os2 << "TRX_STATUS: " << static_cast<unsigned int>(regVal) << "\n";
		if(regVal & TRX_STATUS_MASK_CCA_DONE)
			break;
	}

	forceValidState();

	//check the result
	result = regVal & TRX_STATUS_MASK_CCA_STATUS;

	return SUCESS;
}

void AT86RF230::intHandler(unsigned int){

	/*unsigned char val = AT86RF230_HAL::readRegister(AT86RF230_HAL::IRQ_STATUS_REG);

	//os2 << "#INT=" << val << "#";

	if (val & IRQ_STATUS_RX_START) {

	}
	else if (val & IRQ_STATUS_TRX_END){
		if(sendingData)
			int_trx_end.v();
		else{
			//go back to TRX_OFF
			AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_TRX_OFF);

			_dataReceived = true;
		}
	}
	else if (val & IRQ_STATUS_PLL_LOCK){
		//mutex_pll.p();
		pll_locked = true;
		//mutex_pll.v();
	}
	else if (val & IRQ_STATUS_PLL_UNLOCK){
		//mutex_pll.p();
		pll_locked = false;
		//mutex_pll.v();
	}
	else if(val & IRQ_STATUS_TRX_UR){
		//does'n handle frame buffer access violation
	}
	else if(val & IRQ_STATUS_BAT_LOW){
		//Disable BAT_LOW interrupt to prevent interrupt storm. The interrupt
		//will continously be signaled when the supply voltage is less than the
		//user defined voltage threshold.
		unsigned char trx_isr_mask = AT86RF230_HAL::readRegister(AT86RF230_HAL::IRQ_MASK_REG);
		trx_isr_mask &= ~IRQ_STATUS_BAT_LOW;
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::IRQ_MASK_REG, trx_isr_mask);
	}*/


}




}
