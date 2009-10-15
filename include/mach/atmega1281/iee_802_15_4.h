#ifndef __iee802_15_4_h
#define __iee802_15_4_h

#include <nic.h>
#include <cpu.h>
#include <utility/crc.h>
#include <mach/atmega1281/at86rf230/at86rf230.h>

namespace System {

class IEE_802_15_4: public Low_Power_Radio {

public:
	static unsigned int myAddress;
	static bool _ack_needed;

	enum {
		MAX_FRAME_FULL_SIZE = 127,
		FRAME_DATA_SIZE = 7,
		FRAME_ACK_SIZE = 3,
		FRAME_CHECK_SEQUENCE_SIZE = 2,
		ACK_MAX_WAIT_DURATION = 120000
	//ms
	};

	//	struct frame {
	//		unsigned frameType :3;
	//		unsigned securityEnable :1;
	//		unsigned framePending :1;
	//		unsigned ackRequest :1;
	//		unsigned intraPan :1;
	//		unsigned reserved1 :3;
	//		unsigned destinationAddressingMode :2;
	//		unsigned reserved2 :2;
	//		unsigned sourceAddressingMode :2;
	//		unsigned sequenceNumber :8; //SequenceNumber shall be initiated with a random value see pg.114
	//		unsigned destinationPanIdentifier :16; //TODO Isso só será declarado quando o destinationAddressingMode!=0
	//		unsigned destinationAddress :16;////TODO Isso só será declarado quando o destinationAddressingMode!=0
	//		//unsigned destinationAddress :64;//TODO Se DestinationAddressingMode=2->declarar o de cima, senão esse
	//		unsigned sourcePanIdentifier :16;//TODO Só será declarado se sourceAddressingMode!=0 && intraPan=0
	//		unsigned sourceAddress :16;////TODO Isso só será declarado quando o destinationAddressingMode!=0
	//		//unsigned sourceAddress :64;//TODO Se DestinationAddressingMode=2->declarar o de cima, senão esse
	//	} frame;

	struct frame_data_s {
		unsigned frameType :3;
		unsigned securityEnable :1;
		unsigned framePending :1;
		unsigned ackRequest :1;
		unsigned intraPan :1;
		unsigned reserved1 :3;
		unsigned destinationAddressingMode :2;
		unsigned reserved2 :2;
		unsigned sourceAddressingMode :2;
		unsigned sequenceNumber :8; //SequenceNumber shall be initiated with a random value see pg.114
		unsigned destinationAddress :16;////TODO Isso só será declarado quando o destinationAddressingMode!=0
		unsigned sourceAddress :16;////TODO Isso só será declarado quando o destinationAddressingMode!=0
	} frame_data; //7bytes

	struct frame_ack_s {
		unsigned frameType :3;
		unsigned securityEnable :1;
		unsigned framePending :1;
		unsigned ackRequest :1;
		unsigned intraPan :1;
		unsigned reserved1 :3;
		unsigned destinationAddressingMode :2;
		unsigned reserved2 :2;
		unsigned sourceAddressingMode :2;
		unsigned sequenceNumber :8; //SequenceNumber shall be initiated with a random value see pg.114
	} frame_ack; //3bytes

	enum {
		FRAME_TYPE_BEACON = 0,
		FRAME_TYPE_DATA = 1,
		FRAME_TYPE_ACK = 2,
		FRAME_TYPE_MAC_COMMAND = 3,
		SECURITY_ENABLED_ON = 1,
		SECURITY_ENABLED_OFF = 0,
		FRAME_PENDING_ON = 1,
		FRAME_PENDING_OFF = 0,
		ACK_REQUEST_ON = 1,
		ACK_REQUEST_OFF = 0,
		INTRA_PAN_SAME_PAN = 1,
		INTRA_PAN_OTHER_PAN = 0,//PanIdentifier Field needed
		ADRESSING_MODE_PAN_AND_ADDRESS_NOT_PRESENT = 0,
		ADRESSING_MODE_RESERVED = 1,
		ADRESSING_MODE_SHORT_ADDRESS = 2,//16bits
		ADRESSING_MODE_EXTENDED_ADDRESS = 3,
	//64bits
	//64bits
	};

public:

	IEE_802_15_4();

	int send(const Address & dst, const Protocol & prot, const void *data,
			unsigned int size);

	int receive(Address * src, Protocol * prot, void * data,
			unsigned int size);

	int receive(Address * src, Protocol * prot, void * data,
			unsigned int size, long receivingTime);

	const Address & address();

	const Statistics & statistics();

	void reset();

	void config(int frequency, int power);

	unsigned int mtu() const;

	static void init(unsigned int n);

private:

	int csma_ca_send(const Address & dst, const Protocol & prot,
			const void *data, unsigned int size);

	int _send_data(const Address & dst, const Protocol & prot,
			const void *data, unsigned int size);

	int _send_ack(int senderMacDSN);

private:

	static AT86RF230 radio;

	static volatile bool _tx_available;

	static volatile bool _rx_available;

	//Variables of CSMA-CA
	static bool slotted;
	static int aUnitBackoffPeriod;
	static int NB; //Number of retrys in each transmission
	//static volatile int CW; //Only Used in Slotted CSMA-CA
	static int BE; //Backoff Exponent - how many backoffs the channel must be clean before transmitting
	static int macMinBE;
	static int aMaxBE;
	static int macMaxCSMABackoffs;
	static unsigned char macDSN;

	static bool _im_coordinator;
	//static volatile Frame _tx_frame;


	static unsigned char buffer[MAX_FRAME_FULL_SIZE];

};

}

#endif
