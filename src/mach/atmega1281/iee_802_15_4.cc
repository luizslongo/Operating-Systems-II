#include <mach/atmega1281/iee_802_15_4.h>
#include <alarm.h>
#include <mach/atmega1281/at86rf230/at86rf230.h>
//#include <stdlib.h>
//#include <math.h>
#include <utility/debug.h>
#include <utility/crc.h>

__BEGIN_SYS

/***************************************************************
 *
 * PROTOCOL PUBLIC INTERFACE
 *
 ***************************************************************
 * */

OStream os;
unsigned int IEE_802_15_4::myAddress;
bool IEE_802_15_4::_ack_needed;

IEE_802_15_4::IEE_802_15_4() {
}

const IEE_802_15_4::Address & IEE_802_15_4::address() {
}

const IEE_802_15_4::Statistics & IEE_802_15_4::statistics() {
}

void IEE_802_15_4::reset() {
}

void IEE_802_15_4::config(int frequency, int power) {
}

unsigned int IEE_802_15_4::mtu() const {
	return 0;
}

void IEE_802_15_4::init(unsigned int n) {
	myAddress = n;
	//srand(myAddress);//Different seed to any node
	//macDSN = rand();
}
int IEE_802_15_4::send(const Address & dst, const Protocol & prot,
		const void *data, unsigned int size) {
	return csma_ca_send(dst, prot, data, size);
}

int IEE_802_15_4::receive(Address * src, Protocol * prot, void * data,
		unsigned int size) {
    return IEE_802_15_4::receive(src, prot, data, 0);
}
int IEE_802_15_4::receive(Address * src, Protocol * prot, void * data,
		unsigned int size, long receivingTime) {
	AT86RF230::result_t result;
	if (_rx_available) {
		_rx_available = false;
		result = radio.rx_on();
		if (receivingTime == 0) {
			while (!radio.dataReceived())
				;
		} else {
			Alarm::delay(receivingTime);
			if (!radio.dataReceived()) {
				while ((radio.rx_off() == radio.BUSY)) {
				}
				return 0;//TODO return error
			}
		}
		radio.rx_off();
		int frameBufferSize = 0;
		result = radio.receive(buffer, frameBufferSize);
		if (result == AT86RF230::SUCESS) {
			unsigned short crc = CRC::crc16((reinterpret_cast<char*> (buffer)),
					frameBufferSize - 2);
			unsigned char *framePtr = reinterpret_cast<unsigned char*> (&crc);
			if (buffer[frameBufferSize - 2] == framePtr[0]
					&& buffer[frameBufferSize - 1] == framePtr[1]) {
				int frameSize;
				if (frameBufferSize == 5) {
					frameSize = IEE_802_15_4::FRAME_ACK_SIZE;
					IEE_802_15_4::frame_ack_s
							*frame_ack_ =
									reinterpret_cast<IEE_802_15_4::frame_ack_s*> (buffer);
					if (frame_ack_->sequenceNumber == macDSN) {
						return 1;//success
					}
				} else { //DATA
					frameSize = IEE_802_15_4::FRAME_DATA_SIZE;
					IEE_802_15_4::frame_data_s
							*frame_data_ =
									reinterpret_cast<IEE_802_15_4::frame_data_s*> (buffer);
					if (frame_data_->destinationAddress == myAddress) {
						size = frameBufferSize
								- IEE_802_15_4::FRAME_CHECK_SEQUENCE_SIZE
								- frameSize;
						for (unsigned int var = 0; var < size; ++var) {
							reinterpret_cast<char *>(data)[var] = buffer[frameSize + var];
						}

						if (frame_data_->ackRequest == ACK_REQUEST_ON) {
							_send_ack(frame_data_->sequenceNumber);
						}
                                                return 1;

					} else {
						return 0;
					}
				}
			} else {
			}
		} else {
		}
	}
	_rx_available = true;
	return 0;
}

/***************************************************************
 *
 * PROTOCOL PRIVATE INTERFACE
 *
 ***************************************************************
 * */

int IEE_802_15_4::csma_ca_send(const Address & dst, const Protocol & prot,
		const void *data, unsigned int size) {
	macDSN++;
	NB = 0;
	BE = macMinBE;
	if (_tx_available) {
		if (!slotted) {
			while (NB < macMaxCSMABackoffs) {
				//srand(myAddress);//Different seed to any node
				//int delay = static_cast<int> (fmod(rand(), (pow(2, BE) - 1))
					//	* aUnitBackoffPeriod);
				//delay = (delay < 1000) ? 1000 : delay;
				//Alarm::delay(delay);//random(2^BE -1)*Period
				//Clear Channel Assesment
				bool aux = false;
				radio.CCA_measurement(aux);
				if (aux) {
					_send_data(dst, prot, data, size);
					if (_ack_needed) {
						if (receive(0, 0, 0, 0, 120000l) == 1)//TODO receber por 120ms, e agora?
							return 1;//Success
						aux = false;
					}
				}
				NB = NB + 1;
				BE = BE + 1;
				if (BE > aMaxBE)
					BE = aMaxBE;
			}
		} else { //Slotted
			return 0;
		}
	}
	return 0;//Failure
}

int IEE_802_15_4::_send_data(const Address & dst, const Protocol & prot,
		const void *data, unsigned int size) {

	if (_ack_needed)
		IEE_802_15_4::frame_data.ackRequest = IEE_802_15_4::ACK_REQUEST_ON;
	else
		IEE_802_15_4::frame_data.ackRequest = IEE_802_15_4::ACK_REQUEST_OFF;

	IEE_802_15_4::frame_data.destinationAddress = dst;
	IEE_802_15_4::frame_data.destinationAddressingMode
			= IEE_802_15_4::ADRESSING_MODE_SHORT_ADDRESS;
	IEE_802_15_4::frame_data.sourceAddressingMode
			= IEE_802_15_4::ADRESSING_MODE_SHORT_ADDRESS;
	IEE_802_15_4::frame_data.sourceAddress = myAddress;

	IEE_802_15_4::frame_data.frameType = IEE_802_15_4::FRAME_TYPE_DATA;
	IEE_802_15_4::frame_data.framePending = IEE_802_15_4::FRAME_PENDING_OFF;
	IEE_802_15_4::frame_data.intraPan = IEE_802_15_4::INTRA_PAN_SAME_PAN;
	IEE_802_15_4::frame_data.securityEnable
			= IEE_802_15_4::SECURITY_ENABLED_OFF;

	IEE_802_15_4::frame_data.sequenceNumber = macDSN;//Necessary for ack

	int frameSize = FRAME_DATA_SIZE;
	int totalSize = frameSize + size;
	unsigned char *framePtr =
			reinterpret_cast<unsigned char*> (&(IEE_802_15_4::frame_data));
	for (int var = 0; var < frameSize; ++var) {
		buffer[var] = framePtr[var];
	}

	const char *data2 = static_cast<const char*> (data);
	int var2 = 0;
	if (totalSize < IEE_802_15_4::MAX_FRAME_FULL_SIZE) {
		for (int var = frameSize; var < totalSize; ++var, ++var2) {
			buffer[var] = data2[var2];
		}
	}

	unsigned short crc =
			CRC::crc16(reinterpret_cast<char*> (buffer), totalSize);
	unsigned char *framePtr2 = reinterpret_cast<unsigned char*> (&crc);
	totalSize = totalSize + IEE_802_15_4::FRAME_CHECK_SEQUENCE_SIZE;
	buffer[totalSize - 2] = framePtr2[0];
	buffer[totalSize - 1] = framePtr2[1];

	if (totalSize < IEE_802_15_4::MAX_FRAME_FULL_SIZE) {
		radio.send(buffer, totalSize);
	}

	return 0;
}

int IEE_802_15_4::_send_ack(int senderMacDSN) {

	IEE_802_15_4::frame_ack.sequenceNumber = senderMacDSN;
	IEE_802_15_4::frame_ack.frameType = IEE_802_15_4::FRAME_TYPE_ACK;
	IEE_802_15_4::frame_ack.securityEnable = IEE_802_15_4::SECURITY_ENABLED_OFF;
	IEE_802_15_4::frame_ack.framePending = IEE_802_15_4::FRAME_PENDING_OFF;
	IEE_802_15_4::frame_ack.ackRequest = IEE_802_15_4::ACK_REQUEST_OFF;
	IEE_802_15_4::frame_ack.intraPan = IEE_802_15_4::INTRA_PAN_SAME_PAN;
	IEE_802_15_4::frame_ack.destinationAddressingMode
			= IEE_802_15_4::ADRESSING_MODE_SHORT_ADDRESS;
	IEE_802_15_4::frame_ack.sourceAddressingMode
			= IEE_802_15_4::ADRESSING_MODE_SHORT_ADDRESS;
	int totalFrameSize = IEE_802_15_4::FRAME_ACK_SIZE
			+ IEE_802_15_4::FRAME_CHECK_SEQUENCE_SIZE;
	unsigned char ackBuffer[totalFrameSize];
	unsigned char *framePtr =
			reinterpret_cast<unsigned char*> (&(IEE_802_15_4::frame_ack));
	for (int var = 0; var < IEE_802_15_4::FRAME_ACK_SIZE; ++var) {
		ackBuffer[var] = framePtr[var];
	}

	unsigned short crc = CRC::crc16(reinterpret_cast<char*> (ackBuffer),
			IEE_802_15_4::FRAME_ACK_SIZE);
	unsigned char *framePtr2 = reinterpret_cast<unsigned char*> (&crc);
	ackBuffer[totalFrameSize - 2] = framePtr2[0];
	ackBuffer[totalFrameSize - 1] = framePtr2[1];

	radio.send(ackBuffer, totalFrameSize);

    return 1;
}

volatile bool IEE_802_15_4::_tx_available = true;
volatile bool IEE_802_15_4::_rx_available = false;

AT86RF230 IEE_802_15_4::radio;

//Variables of CSMA-CA
bool IEE_802_15_4::slotted = false; //TODO Colocar tudo isso em constants
int IEE_802_15_4::aUnitBackoffPeriod = 20000;//FIXME 20 value confirmed, but is to low for EPOS
int IEE_802_15_4::NB = 0;
int IEE_802_15_4::BE = 0;
int IEE_802_15_4::macMinBE = 3;
int IEE_802_15_4::aMaxBE = 5;
int IEE_802_15_4::macMaxCSMABackoffs = 8;//FIXME confirm this value
unsigned char IEE_802_15_4::macDSN;//This is inittiated with a random value

unsigned char IEE_802_15_4::buffer[IEE_802_15_4::MAX_FRAME_FULL_SIZE];

//Configs
bool IEE_802_15_4::_im_coordinator = false;

__END_SYS
