// EPOS-- ATMega1281_Transceiver Init
#include <machine.h>

namespace System {

void ATMega1281_Transceiver::init() {

	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::init - setting up HAL\n";
	AT86RF230_HAL::init();
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::init - setting up interrupts\n";
	AT86RF230_HAL::setInterruptHandler(&intHandler);
	AT86RF230_HAL::initInterrupts();
	//force the radio to a valid state
	db<ATMega1281_Transceiver>(INF) << "ATMega1281_Transceiver::init - forcing to a valid state\n";
	AT86RF230_HAL::writeRegister(AT86RF230_HAL::TRX_STATE_REG, TRX_CMD_FORCE_TRX_OFF);

}

}

