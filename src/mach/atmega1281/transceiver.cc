/*
 * transceiver.cc
 *
 *  Created on: 27/01/2009
 *      Author: tiago
 */

#include <machine.h>

namespace System {

unsigned char ckp_count = 0;
inline void ATMega1281_Transceiver_checkpoint(){
	ckp_count = (ckp_count > 7)?0:(ckp_count+1);

	CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
	CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (ckp_count << 5));
}

//initialize static members
ATMega1281_Transceiver::operating_mode_t ATMega1281_Transceiver::operatting_mode = ATMega1281_Transceiver::BASIC_OPERATING_MODE;

Transceiver_Common::event_handler* ATMega1281_Transceiver::ev_handler = 0;


ATMega1281_Transceiver::result_t ATMega1281_Transceiver::forceValidState(){
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::forceValidState - forcing a valid state\n";

	operatting_mode = ATMega1281_Transceiver::BASIC_OPERATING_MODE;

	//kout << "rfvsi\n";
	while (getState() != TRX_STATUS_TRX_OFF)
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);
	//kout << "rfvso\n";

	return SUCCESS;
}

void ATMega1281_Transceiver::intEnable(){
	AT86RF230_HAL::interruptEnable();

}

void ATMega1281_Transceiver::intDisable(){
	AT86RF230_HAL::interruptDisable();
}

ATMega1281_Transceiver::state_t ATMega1281_Transceiver::getState(){
	unsigned char value = AT86RF230_HAL::readRegister(AT86RF230_HAL::TRX_STATUS_REG);
	value &= TRX_STATUS_MASK_TRX_STATUS;
	return static_cast<ATMega1281_Transceiver::state_t>(value);
}

ATMega1281_Transceiver::operating_mode_t ATMega1281_Transceiver::getOperatingMode(){
	//TODO implement
	return BASIC_OPERATING_MODE;
}

ATMega1281_Transceiver::result_t ATMega1281_Transceiver::setOperatingMode(){
	//TODO implement
	return FAILED;
}

volatile bool sending = false;

ATMega1281_Transceiver::result_t ATMega1281_Transceiver::send(unsigned char *buffer, int size){
	if(getState() != ATMega1281_Transceiver::TRX_STATUS_TRX_OFF){
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - radio busy\n";
		return BUSY;
	}

	if(operatting_mode == ATMega1281_Transceiver::BASIC_OPERATING_MODE){
		sending = true;

		AT86RF230_HAL::Frame frame;
		frame.data = buffer;
		frame.frame_length = size;

		//write data to the frame buffer
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - writing to frame buffer\n";
		AT86RF230_HAL::writeFrameBuffer(&frame);

		//go to PLL_ON state and wait for pll to lock
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - going to PLL_ON state\n";
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_PLL_ON);
		while(getState() != TRX_STATUS_PLL_ON);

		//starting TX
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - transmitting\n";
		AT86RF230_HAL::setSLP_TRhigh();
		AT86RF230_HAL::setSLP_TRlow();

		//should be back to PLL_ON state when TX end
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - returning to PLL_ON\n";
		while(getState() != TRX_STATUS_PLL_ON);

		//return to TRX_OFF
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - returning to TRX_OFF\n";
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_TRX_OFF);

		sending = false;

		if(getState() != TRX_STATUS_TRX_OFF){
			return FAILED;
		}
		else
			return SUCCESS;
	}
	else{ //extended operating mode
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::send - extended operating mode not implemented\n";
		//TODO implement
		return FAILED;
	}
}


ATMega1281_Transceiver::result_t ATMega1281_Transceiver::rx_on(){

	if(getState() != TRX_STATUS_TRX_OFF){
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::rx_on - radio busy\n";
		return BUSY;
	}

	if(operatting_mode == ATMega1281_Transceiver::BASIC_OPERATING_MODE){

		//go to RX_ON and wait pll to lock
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::rx_on - going to state RX_ON\n";
		while(getState() != TRX_STATUS_RX_ON){
			AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_RX_ON);
		}
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::rx_on - PLL locked\n";

		if(getState() != TRX_STATUS_RX_ON)
			return FAILED;

		return SUCCESS;
	}
	else{ //extended operating mode
		//TODO implement
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::rx_on - extended operating mode not implemented\n";
		return FAILED;
	}
}

ATMega1281_Transceiver::result_t ATMega1281_Transceiver::rx_off(){

	state_t state = getState();

	if(state == TRX_STATUS_BUSY_RX){
		return BUSY;
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::rx_off - extended operating mode not implemented\n";
	}
	else{
		return forceValidState();
	}
}

ATMega1281_Transceiver::result_t ATMega1281_Transceiver::receive(unsigned char *buffer, int &size){

	if(getState() != TRX_STATUS_TRX_OFF){
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::receive - radio busy\n";
		return BUSY;
	}

	AT86RF230_HAL::Frame frame;
	frame.data = buffer;

	//read the received data
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::receive - reading from frame buffer\n";
	AT86RF230_HAL::readFrameBuffer(&frame);

	size = frame.frame_length;

	return SUCCESS;
}


ATMega1281_Transceiver::result_t ATMega1281_Transceiver::CCA_measurement(bool &result, CCA_mode_t mode, unsigned char ED_threshold){

    if(getState() != TRX_STATUS_TRX_OFF){
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::CCA_measurement - radio busy\n";

	return forceValidState();
    }

	//set the energy threshold
	//????

	//set cca mode and request
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::CCA_measurement - setting CCA mode\n";
	unsigned char regVal = AT86RF230_HAL::readRegister(AT86RF230_HAL::PHY_CC_CCA_REG);

	regVal &= ~PHI_CC_CCA_MASK_CCA_MODE;
	regVal |= (mode << 5);
	regVal |= PHI_CC_CCA_MASK_CCA_REQUEST;

	//start the measurement
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::CCA_measurement - starting measurement\n";
	result_t aux = rx_on();
	if(aux != SUCCESS){
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::CCA_measurement - measurement failed\n";

		return forceValidState();
	}
	AT86RF230_HAL::writeRegister(AT86RF230_HAL::PHY_CC_CCA_REG, regVal);

	//poll the radio to check if the measurement is finished
	while(true){
		regVal = AT86RF230_HAL::readRegister(AT86RF230_HAL::TRX_STATUS_REG);
		if(regVal & TRX_STATUS_MASK_CCA_DONE)
			break;
	}
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::CCA_measurement - measurement finished\n";

	//check the result
	result = regVal & TRX_STATUS_MASK_CCA_STATUS;

	return forceValidState();
}

void ATMega1281_Transceiver::set_event_handler(Transceiver_Common::event_handler* handler){
	ev_handler = handler;
}

Transceiver_Common::event_handler* ATMega1281_Transceiver::get_event_handler(){
	return ev_handler;
}

void ATMega1281_Transceiver::intHandler(){

	CPU::int_disable();

	unsigned char val = AT86RF230_HAL::readRegister(AT86RF230_HAL::IRQ_STATUS_REG);

	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - handling interrupt\n";


	if (val & IRQ_STATUS_TRX_END) {
		//kout << "TRX\n";
	    if (ev_handler != 0) {
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - TRX_END, calling event handler\n";
		if(sending) //this is related to a transmited frame
			(ev_handler(FRAME_TRANSMITED));
		else
			(ev_handler(FRAME_RECEIVED));
	    }
	}
	else if (val & IRQ_STATUS_BAT_LOW){
		//kout << "BAT\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - BAT_LOW, disabling interrupt\n";
		//Disable BAT_LOW interrupt to prevent interrupt storm. The interrupt
		//will continously be signaled when the supply voltage is less than the
		//user defined voltage threshold.
		unsigned char trx_isr_mask = AT86RF230_HAL::readRegister(AT86RF230_HAL::IRQ_MASK_REG);
		trx_isr_mask &= ~IRQ_STATUS_BAT_LOW;
		AT86RF230_HAL::writeRegister(AT86RF230_HAL::IRQ_MASK_REG, trx_isr_mask);

	}
	else if (val & IRQ_STATUS_PLL_LOCK){
		//kout << "PL\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - PLL_LOCK, not handled\n";
		//that's OK
	}
	else if (val & IRQ_STATUS_PLL_UNLOCK){
		//kout << "PU\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - PLL_UNLOCK, not handled\n";
		//nothing to do, in the next transmit/receive operation it will lock again

	}
	else if(val & IRQ_STATUS_TRX_UR){
		//kout << "UR\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - TRX_UR, not handled\n";
		//does'n handle frame buffer access violation
	}
	else if(val & IRQ_STATUS_RX_START){
		//kout << "RX\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - RX_START, not handled\n";
		//nothing to do
	}
	else{
		//kout << "UK\n";
		db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::intHandler - unknown interrupt " << val << "\n";
	}

	AT86RF230_HAL::interruptEnable();
	CPU::int_enable();

}

void ATMega1281_Transceiver::hardware_reset(){
	AT86RF230_HAL::hw_reset();
}

}
