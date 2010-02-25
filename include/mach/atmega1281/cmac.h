#ifndef __cmac_h
#define __cmac_h

#include <nic.h>
#include <radio.h>
#include "transceiver.h"
#include <cpu.h>
#include <utility/crc.h>
#include <utility/handler.h>
#include <utility/crc.h>
//#include <semaphore.h>
#include <utility/random.h>

__BEGIN_SYS

class CMAC: public Low_Power_Radio {
public:
	enum CMAC_STATE {
		OFF,
		SYNC,
		PACK, CONTENTION, TX, ACK_RX,  	// send //ACK_RX=101
		LPL, RX, UNPACK, ACK_TX	// receive LPL=6 //ACK_TX=1001
	};

	enum CMAC_STATE_TRANSITION {
		SYNC_END, TIMER_INT,
		// send
		TX_PENDING, PACK_OK, PACK_FAILED, CHANNEL_BUSY, CHANNEL_IDLE, TX_END, TX_ERROR, TX_OK, TX_FAILED,
		// receive
		RX_PENDING, PREAMBLE_DETECTED, TIMEOUT, RX_END, RX_ERROR, UNPACK_OK, UNPACK_FAILED, RX_OK
	};

	enum {
		FRAME_BUFFER_SIZE = Transceiver::MAX_FRAME_SIZE
	};

public:
	CMAC(int unit = 0) {   }

	int send(const Address & dst, const Protocol & prot,
			const void *data, unsigned int size);

	int receive(Address * src, Protocol * prot,
			void * data, unsigned int size);

	const Address & address() {
		return _addr;
	}

	/*
	 * For CMAC, we use the following statistics:
	 * (all the statistics are related only to data packets)
	 *
	 * rx_packets = number of packets successfully received
	 * rx_bytes = number of bytes successfully received
	 * rx_time = time spent receiving packets
	 * tx_packets = number of packets successfully transmitted
	 * tx_bytes = number of bytes successfully transmitted
	 * tx_time = time spent transmitting packets
	 * dropped_packets = number of packets dropped due to errors
	 * total_tx_packets = total number of packets transmitted
	 */
	const Statistics & statistics() {
		return (Statistics&)_stats;
	}

	void reset() {
	}

	void config(int frequency, int power) {
	}

	unsigned int mtu() const { return Traits<CMAC>::MTU; }

	static void init(unsigned int n);

	template<class Sync,
	class Pack, class Contention, class Tx, class Ack_Rx,
	class Lpl, class Rx, class Unpack, class Ack_Tx>
	static CMAC_STATE_TRANSITION state_machine();

	static void state_machine_handler();


private://timing control
	typedef void (event_handler)(void);

	static event_handler *alarm_ev_handler;
	//static Handler_Function alarm_handler;
	static volatile unsigned long alarm_ticks_ms;
	static unsigned long alarm_event_time_ms;
	//static Alarm alarm;
	static Timer_2 timer;
	//static Semaphore timer_int;
	static volatile bool timeout;

	//static void alarm_handler_int_handler(unsigned int);

	//friend class Thread;
	//static int alarm_thread_entry();

	static void alarm_handler_function();

	static void alarm_event_time(unsigned long event_time){
		alarm_event_time_ms = alarm_ticks_ms + event_time;
	}

	static unsigned long alarm_event_time_left(){
		return alarm_ticks_ms - alarm_event_time_ms;
	}

	static void alarm_busy_delay(unsigned long time){
		unsigned long delay = alarm_ticks_ms + time;
		while(delay <= alarm_ticks_ms);
	}

/*	static unsigned long alarm_time_elapsed(unsigned long reference){
		if(alarm_ticks_ms >= reference)
			return alarm_event_time_ms - reference;
		else
			return (0xFFFFFFFC - reference) + alarm_ticks_ms;
	}*/

	static void alarm_activate(event_handler * handler, unsigned long period) {
		alarm_ev_handler = handler;
		alarm_event_time(period);
	}

	static void alarm_deactivate() {
		alarm_ev_handler = 0;
	}

public:
	static unsigned long alarm_time(){
		return alarm_ticks_ms;
	}

private://for debugging
	static void sm_step_int_handler();

	static volatile bool sm_step_next_step;

	static void sm_step(){
		if(Traits<CMAC>::SM_STEP_DEBUG){
			//writes on the leds the next state and waits for a button pressed int
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (_state << 5));
			while(!sm_step_next_step);
			sm_step_next_step = false;
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
		}
	}


private:
	static Transceiver radio;

	//timer control variables
	static unsigned int _sleeping_period; //ms

	static Address             _addr;
	static volatile Statistics _stats;
	static volatile CMAC_STATE _state;

	//used only when TIME_TRIGGERED = true
	static Semaphore _sem_rx, _sem_tx;
	static CMAC_STATE_TRANSITION _state_machine_result;
	static volatile bool _last_sm_exec_tx; //true if the last execution of the state machine executed an TX
	static volatile bool _last_sm_exec_rx; //true if the last execution of the state machine executed an RX
	static volatile bool _on_active_cycle;

	static unsigned char _frame_buffer[FRAME_BUFFER_SIZE];
	static unsigned int _frame_buffer_size;

	static void *_rx_data;
	static const void *_tx_data;
	static unsigned int _rx_data_size;
	static unsigned int _tx_data_size;
	static Address _tx_dst_address;
	static Address _rx_src_address;
	static volatile bool _rx_pending;
	static volatile bool _tx_pending;
	static int _transmission_count;

	static unsigned char _data_sequence_number;

	static int _consecutive_failures;

	friend class CMAC_States::Empty;
	friend class CMAC_States::Sync_Empty;
	friend class CMAC_States::Pack_Empty;
	friend class CMAC_States::Contention_Empty;
	friend class CMAC_States::Tx_Empty;
	friend class CMAC_States::Ack_Rx_Empty;
	friend class CMAC_States::Lpl_Empty;
	friend class CMAC_States::Rx_Empty;
	friend class CMAC_States::Unpack_Empty;
	friend class CMAC_States::Ack_Tx_Empty;

	friend class CMAC_States::Generic_Sync;
	friend class CMAC_States::Generic_Tx;
	friend class CMAC_States::Generic_Rx;
	friend class CMAC_States::Generic_Lpl;
	friend class CMAC_States::Unslotted_CSMA_Contention;

	friend class CMAC_States::IEEE802154_Beacon_Sync;
	friend class CMAC_States::IEEE802154_Pack;
	friend class CMAC_States::IEEE802154_Unpack;
	friend class CMAC_States::IEEE802154_Ack_Rx;
	friend class CMAC_States::IEEE802154_Ack_Tx;
	friend class CMAC_States::IEEE802154_Slotted_CSMA_Contention;
};

// CMAC states implementation
namespace CMAC_States
{

/*
 * Empty states implementation
 */

class Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Empty - UNPACK_FAILED\n";
		return CMAC::UNPACK_FAILED;
	}
};

class Sync_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		if(CMAC::_rx_pending){
			db<CMAC>(TRC) << "CMAC_States::Sync_Empty - RX_PENDING\n";
			return CMAC::RX_PENDING;
		}
		else if(CMAC::_tx_pending){
			db<CMAC>(TRC) << "CMAC_States::Sync_Empty - TX_PENDING\n";
			return CMAC::TX_PENDING;
		}
		else{
			db<CMAC>(TRC) << "CMAC_States::Sync_Empty - SYNC_END\n";
			return CMAC::SYNC_END;
		}
	}
};

class Pack_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Pack_Empty - PACK_OK\n";
		CMAC::_transmission_count += 1;
		return CMAC::PACK_OK;
	}
};

class Contention_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Contention_Empty - CHANNEL_IDLE\n";
		return CMAC::CHANNEL_IDLE;
	}
};

class Tx_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Tx_Empty - TX_END\n";
		return CMAC::TX_END;
	}
};

class Ack_Rx_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Ack_Rx_Empty - TX_OK\n";
		CMAC::_transmission_count = 0;
		return CMAC::TX_OK;
	}
};

class Lpl_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Lpl_Empty - PREAMBLE_DETECTED\n";
		return CMAC::PREAMBLE_DETECTED;
	}
};

class Rx_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Rx_Empty - RX_END\n";
		char data[16] = "RX_Empty State\n";
		CMAC::_rx_data_size = 16;
		char *aux = reinterpret_cast<char*>(CMAC::_rx_data);
		for (int i = 0; i < 16; ++i) {
			aux[i] = data[i];
		}
		return CMAC::RX_END;
	}
};

class Unpack_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Unpack_Empty - UNPACK_OK\n";
		return CMAC::UNPACK_OK;
	}
};

class Ack_Tx_Empty {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		db<CMAC>(TRC) << "CMAC_States::Ack_Tx_Empty - RX_OK\n";
		return CMAC::RX_OK;
	}
};


/*
 * Generic states implementation
 */

class Generic_Sync {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::Generic_Sync - Setting timeout\n";

		CMAC::timeout = false;
		CMAC::alarm_activate(&alarm_handler, Traits<CMAC>::TIMEOUT);

		if(CMAC::_rx_pending){
			db<CMAC>(TRC) << "CMAC_States::Generic_Sync - RX_PENDING\n";
			return CMAC::RX_PENDING;
		}
		else if(CMAC::_tx_pending){
			db<CMAC>(TRC) << "CMAC_States::Generic_Sync - TX_PENDING\n";
			return CMAC::TX_PENDING;
		}
		else{
			db<CMAC>(TRC) << "CMAC_States::Generic_Sync - SYNC_END\n";
			return CMAC::SYNC_END;
		}
	}

private:
	static void alarm_handler(){
		db<CMAC>(INF) << "CMAC_States::Generic_Sync - timeout\n";
		while(!CMAC::timeout) CMAC::timeout = true;//no excuses now
		//CMAC::alarm_deactivate();
	}
};

class Generic_Tx {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		//According to the state machine, TX should never fail, but hardware failure may happen

		Transceiver::result_t result;

		for (int var = 0; var < 5; ++var) {
			db<CMAC>(TRC) << "CMAC_States::Generic_Tx - sending\n";
			result = CMAC::radio.send(CMAC::_frame_buffer, CMAC::_frame_buffer_size);

			if(result != Transceiver::SUCCESS){
				db<CMAC>(WRN) << "CMAC_States::Generic_Tx - Tx failed - Trying again\n";
				CMAC::radio.forceValidState();
			}
			else
				break;
		}

		if(result == Transceiver::SUCCESS){
			db<CMAC>(TRC) << "CMAC_States::Generic_Tx - TX_END\n";
			return CMAC::TX_END;
		}
		else{
			db<CMAC>(ERR) << "CMAC_States::Generic_Tx - TX_ERROR\n";
			return CMAC::TX_ERROR;
		}

	}
};

class Generic_Rx {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		//According to the state machine, RX should never fail, but hardware failure may happen

		Transceiver::result_t result;

		int size = 0;

		db<CMAC>(TRC) << "CMAC_States::Generic_Rx - receiving\n";
		result = CMAC::radio.receive(&(CMAC::_frame_buffer[0]), size);

		if(result != Transceiver::SUCCESS){
			db<CMAC>(ERR) << "CMAC_States::Generic_Rx - RX_ERROR\n";
			CMAC::radio.forceValidState();
			return CMAC::RX_ERROR;
		}

		CMAC::_frame_buffer_size = size;

		db<CMAC>(TRC) << "CMAC_States::Generic_Rx - RX_END\n";
		return CMAC::RX_END;
	}
};

class Generic_Lpl {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {
		return execute(input, 0);
	}

	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input, unsigned long soft_timeout) {

		db<CMAC>(TRC) << "CMAC_States::Generic_Lpl - Listening\n";

		Transceiver::result_t result;

		CMAC::radio.set_event_handler(&_event_handler);

		_frame_received = false;
		result = CMAC::radio.rx_on();


		/*ASMV("mov r0, r0"::);
		unsigned long timeout2 = CMAC::alarm_ticks_ms + soft_timeout;
		ASMV("mov r1, r1"::);
		bool timeout = CMAC::timeout;
		ASMV("mov r2, r2"::);
		while (!timeout && !_frame_received){
			ASMV("mov r3, r3"::);
			if(soft_timeout == 0){
				timeout = CMAC::timeout;
				ASMV("mov r4, r4"::);
			}
			else{
				timeout = (CMAC::alarm_ticks_ms >= timeout2) || CMAC::timeout;
				ASMV("mov r5, r5"::);
			}
			ASMV("mov r6, r6"::);
		}*/
		ASMV("mov r0, r0"::);
		bool timeout;
		if(soft_timeout == 0){
			ASMV("mov r1, r1"::);
			while (!(timeout = CMAC::timeout) && !_frame_received);
			ASMV("mov r2, r2"::);
		}
		else{
			ASMV("mov r3, r3"::);
			unsigned long timeout2 = CMAC::alarm_ticks_ms + soft_timeout;
			while (!(timeout = (CMAC::alarm_ticks_ms >= timeout2) || CMAC::timeout) && !_frame_received);
			ASMV("mov r4, r4"::);
		}

		//listen_sem.p();

		db<CMAC>(INF) << "CMAC_States::Generic_Lpl::event_handler - FRAME_RECEIVED 2 \n";

		CMAC::CMAC_STATE_TRANSITION transition;
		if (timeout) {
			db<CMAC>(WRN) << "CMAC_States::Generic_Lpl - TIMEOUT\n";
			CMAC::radio.rx_off();
			transition = CMAC::TIMEOUT;
		} else  {
			db<CMAC>(INF) << "CMAC_States::Generic_Lpl - PREAMBLE_DETECTED\n";
			transition = CMAC::PREAMBLE_DETECTED;
		}

		return transition;
	}
private:
	static void _event_handler(Transceiver::event_t event){
		if(event == Transceiver::FRAME_RECEIVED){
			CMAC::radio.rx_off();
			while(!_frame_received) _frame_received = true;//no excuses now
			//listen_sem.v();
			db<CMAC>(INF) << "CMAC_States::Generic_Lpl::event_handler - FRAME_RECEIVED" << _frame_received << "\n";
		}
	}

	//static Semaphore listen_sem;
	static volatile bool _frame_received;
};


class Unslotted_CSMA_Contention {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::Unslotted_CSMA_Contention - Checking channel\n";

		unsigned int nb = 0;
		unsigned int be = MIN_BE;

		while (nb < MAX_BACKOFFS) {

			//delay = random(2^BE -1)*Period in ms
			unsigned long delay = static_cast<unsigned long> ((Pseudo_Random::random() % (2 << (be-1))) * UNIT_BACKOFF_PERIOD);

			db<CMAC>(INF) << "CMAC_States::Unslotted_CSMA_Contention - Delay = " << delay << "\n";

			//for some reason the delay can NEVER be 0
			if(delay == 0) delay = static_cast<unsigned long>(UNIT_BACKOFF_PERIOD);

			CMAC::alarm_busy_delay(delay);

			//Clear Channel Assesment(CCA)

			bool aux = false;
			CMAC::radio.CCA_measurement(aux);

			if (aux) {
				db<CMAC>(TRC) << "CMAC_States::Unslotted_CSMA_Contention - CHANNEL_IDLE\n";
				return CMAC::CHANNEL_IDLE;
			}
			nb = nb + 1;
			be = be + 1;
			if (be > MAX_BE)
				be = MAX_BE;
		}

		db<CMAC>(WRN) << "CMAC_States::Unslotted_CSMA_Contention - CHANNEL_BUSY\n";
		return CMAC::CHANNEL_BUSY;
	}

private:
	enum{
		MIN_BE = 3,
		MAX_BE = 5,
		MAX_BACKOFFS = 8,
		UNIT_BACKOFF_PERIOD = 15 //ms
	};
};



/*
 * IEEE 802.15.4 states implementation
 */

class IEEE802154_Frame {
public:
	enum {
			FRAME_TYPE_BEACON = 0,
			FRAME_TYPE_DATA = 1,
			FRAME_TYPE_ACK = 2,
			FRAME_TYPE_MAC_COMMAND = 3, //NOT SUPPORTED
			SECURITY_ENABLED_ON = 1, //NOT SUPPORTED
			SECURITY_ENABLED_OFF = 0, //NOT SUPPORTED
			FRAME_PENDING_ON = 1, //NOT SUPPORTED
			FRAME_PENDING_OFF = 0, //NOT SUPPORTED
			ACK_REQUEST_ON = 1, //NOT SUPPORTED
			ACK_REQUEST_OFF = 0, //NOT SUPPORTED
			INTRA_PAN_SAME_PAN = 1,
			INTRA_PAN_OTHER_PAN = 0,//PanIdentifier Field needed - NOT SUPORTED
			ADDRESSING_MODE_PAN_AND_ADDRESS_NOT_PRESENT = 0, //NOT SUPPORTED
			ADDRESSING_MODE_RESERVED = 1, //NOT SUPPORTED
			ADDRESSING_MODE_SHORT_ADDRESS = 2,//16bits addresses
			ADDRESSING_MODE_EXTENDED_ADDRESS = 3, //NOT SUPPORTED
		};

	typedef struct {
		unsigned frameType :3;
		unsigned securityEnable :1; //NOT SUPPORTED
		unsigned framePending :1; //NOT SUPPORTED
		unsigned ackRequest :1; //NOT SUPPORTED
		unsigned intraPan :1;
		unsigned reserved1 :3; //NOT SUPPORTED
		unsigned destinationAddressingMode :2;
		unsigned reserved2 :2; //NOT SUPPORTED
		unsigned sourceAddressingMode :2;
	} frame_control_t; //2 bytes

	typedef struct {
		frame_control_t frame_control;
		unsigned beacon_sequence_n :8;
		unsigned source_address :16; //Only 16bits addresses suported
		unsigned beacon_order :4;
		unsigned superframe_order :4;
		unsigned final_cap_slot :4; //NOT SUPPORTED
		unsigned batery_life_ext :1; //NOT SUPPORTED
		unsigned reserved :1; //NOT SUPPORTED
		unsigned pan_coordinator :1; //NOT SUPPORTED
		unsigned association_permit :1; //NOT SUPPORTED
		//For the sake of simplicity, the following variable fields are not supported
		//GTS fields
		//Pending address fields
		//Beacon payload field
		unsigned char dummy[6]; //radio workaround, the minimum MAC frame size is 15 (probably a hardware bug)
		unsigned frame_check_sequence :16;
	}beacon_frame_t; // 9 bytes

	typedef struct {
		frame_control_t frame_control;
		unsigned data_sequence_n :8;
		unsigned char dummy[10]; //radio workaround, the minimum MAC frame size is 15 (probably a hardware bug)
		unsigned frame_check_sequence :16;
	}ack_frame_t; //5 bytes

	typedef struct {
		frame_control_t frame_control;
		unsigned data_sequence_n :8;
		//only 16 bits addresses without PAN ID's are suported
		unsigned destination_address :16;
		unsigned source_address :16;
	}data_frame_header_t; //7 bytes

	friend Debug &operator<< (Debug &out, frame_control_t &fc){
		out << "frameType: " << fc.frameType << "\n";
		//<< "securityEnable: " << fc.securityEnable << "\n"
		//<< "framePending: " << fc.framePending << "\n"
		//<< "ackRequest: " << fc.ackRequest << "\n"
		//<< "intraPan: " << fc.intraPan << "\n"
		//<< "destAddrMode: " << fc.destinationAddressingMode << "\n"
		//<< "srcAddrMode: " << fc.sourceAddressingMode << "\n";
		return out;
	}

	friend Debug &operator<< (Debug &out, beacon_frame_t &bc){
		out << bc.frame_control
		<< "beacon_seq_n: " << bc.beacon_sequence_n << "\n"
		<< "src_addr: " << bc.source_address << "\n"
		<< "beacon_order: " << bc.beacon_order << "\n"
		<< "superframe_order: " << bc.superframe_order << "\n"
		//<< "final_cap_slot: " << bc.final_cap_slot << "\n"
		//<< "bat_life_ext: " << bc.batery_life_ext << "\n"
		//<< "pan_coord: " << bc.pan_coordinator << "\n"
		//<< "assoc_permit: " << bc.association_permit << "\n"
		<< "CRC: " << bc.frame_check_sequence << "\n";
		return out;
	}

	friend Debug &operator<< (Debug &out, ack_frame_t &ack){
		out << ack.frame_control
		<< "data_seq_n: " << ack.data_sequence_n << "\n"
		<< "CRC: " << ack.frame_check_sequence << "\n";
		return out;
	}

	friend Debug &operator<< (Debug &out, data_frame_header_t &data_header){
		out << data_header.frame_control
		<< "data_seq_n: " << data_header.data_sequence_n << "\n"
		<< "src_addr: " << data_header.source_address << "\n"
		<< "dst_addr: " << data_header.destination_address << "\n";
		return out;
	}

};

/*class IEEE802154_Beacon_Sync {
public:
	enum{
		MAX_BEACON_ORDER 			= 8, //up to 14
		MAX_SUPERFRAME_ORDER 		= 8,//up to 14

		DEFAULT_BEACON_ORDER	 	= 6,
		DEFAULT_SUPERFRAME_ORDER	= 5,
	};

public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Setting timeout\n";


		if (!_coordinator) {

			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (2 << 5));//led yellow = trying to sync

			//kout << "a\n";

			//receive beacons
			set_time_out();
			CMAC::CMAC_STATE_TRANSITION result = receive_beacon();

			//kout << "b\n";

			if(result == CMAC::TIMEOUT) {

				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (1 << 5));//led red = lost sync

				//listen for a random period
				unsigned long delay =
						static_cast<unsigned long> (Pseudo_Random::random() % ((0x0001 << _superframe_order) * 2));

				CMAC::timeout = false;
				CMAC::alarm_activate(&alarm_handler, delay);

				result = receive_beacon();

				CMAC::timeout = false;
				CMAC::alarm_deactivate();

				//if nothing was received become coordinator
				if(result == CMAC::TIMEOUT){
					db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Became coordinator\n";
					_coordinator = true;

					CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
					CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (7 << 5));//all leds on = coordinator
				}
			}

			//kout << "c\n";

			if(result == CMAC::RX_END){
				set_time_out();
				set_sleeping_period();
				db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Synchronized with coordinator\n";

				//kout << "cc\n";

				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (4 << 5));//leds green = synchronized
			}
		}

		//kout << "d\n";

		if(_coordinator) {
			//send beacons
			_beacon_order = DEFAULT_BEACON_ORDER;
			_superframe_order = DEFAULT_SUPERFRAME_ORDER;
			set_time_out();
			set_sleeping_period();
			_beacon_sequence_n = 0;
			send_beacon();
			send_beacon();
			send_beacon();
			send_beacon();
			//send_beacon(); highly unstable
		}

		//kout << "f\n";

		if(CMAC::_rx_pending){
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - RX_PENDING\n";
			return CMAC::RX_PENDING;
		}
		else if(CMAC::_tx_pending){
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - TX_PENDING\n";
			return CMAC::TX_PENDING;
		}
		else{
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - SYNC_END\n";
			return CMAC::SYNC_END;
		}
	}

private:

	static CMAC::CMAC_STATE_TRANSITION pack_beacon(CMAC::CMAC_STATE_TRANSITION previous){
		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Creating frame\n";

		IEEE802154_Frame::beacon_frame_t *beacon_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::beacon_frame_t*>(CMAC::_frame_buffer);

		CMAC::_frame_buffer_size = sizeof(IEEE802154_Frame::beacon_frame_t);

		beacon_frame_ptr->frame_control.frameType = IEEE802154_Frame::FRAME_TYPE_BEACON;
		beacon_frame_ptr->frame_control.intraPan = IEEE802154_Frame::INTRA_PAN_SAME_PAN;
		beacon_frame_ptr->frame_control.destinationAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;
		beacon_frame_ptr->frame_control.sourceAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;

		beacon_frame_ptr->beacon_sequence_n = ++_beacon_sequence_n;
		beacon_frame_ptr->source_address = CMAC::_addr;
		beacon_frame_ptr->beacon_order = _beacon_order;
		beacon_frame_ptr->superframe_order = _superframe_order;

		beacon_frame_ptr->frame_check_sequence =
				CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Frame created:\n"
					  << *beacon_frame_ptr;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - PACK_OK\n";

		return CMAC::PACK_OK;
	}

	static CMAC::CMAC_STATE_TRANSITION send_beacon(){
		CMAC::CMAC_STATE_TRANSITION result = CMAC::TX_PENDING;

		result = pack_beacon(result);
		result = CMAC_States::Generic_Tx::execute(result);

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - TX_END\n";
		return result;
	}

	static CMAC::CMAC_STATE_TRANSITION unpack_beacon(CMAC::CMAC_STATE_TRANSITION previous){

		if(CMAC::_frame_buffer_size == 0){
			db<CMAC>(ERR) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_FAILED - Frame size == 0\n";
			return CMAC::UNPACK_FAILED;
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Decoding frame\n";

		IEEE802154_Frame::beacon_frame_t *beacon_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::beacon_frame_t*>(CMAC::_frame_buffer);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Frame decoded:\n"
					  << *beacon_frame_ptr;

		unsigned short crc = CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		if(beacon_frame_ptr->frame_check_sequence != crc){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - CRC error: " << crc << "\n";
			return CMAC::UNPACK_FAILED;
		}

		if(beacon_frame_ptr->frame_control.frameType != IEEE802154_Frame::FRAME_TYPE_BEACON){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_FAILED - Incorrect frame type\n";
			return CMAC::UNPACK_FAILED;
		}

		_beacon_sequence_n = beacon_frame_ptr->beacon_sequence_n;
		_beacon_order = beacon_frame_ptr->beacon_order;
		_superframe_order = beacon_frame_ptr->superframe_order;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_OK\n";

		return CMAC::UNPACK_OK;

	}

	static CMAC::CMAC_STATE_TRANSITION receive_beacon(){
		CMAC::CMAC_STATE_TRANSITION result = CMAC::RX_PENDING;

		while (result != CMAC::UNPACK_OK) {
			result = CMAC_States::Generic_Lpl::execute(result);
			if (result == CMAC::TIMEOUT) {
				db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - TIMEOUT\n";
				return CMAC::TIMEOUT;
			}
			result = CMAC_States::Generic_Rx::execute(result);
			result = unpack_beacon(result);
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - RX_END\n";
		return CMAC::RX_END;

	}

	static void alarm_handler(){
		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - timeout\n";
		while(!CMAC::timeout) CMAC::timeout = true;//no excuses now
		//kout << CMAC::alarm_event_time_ms << "\n";
		//CMAC::alarm_deactivate();
	}

	static void set_time_out(){
		//set the active cycle in ms
		unsigned short timeout = 0x0001 << _superframe_order;
		timeout *= 15;
		CMAC::timeout = false;
		//kout << CMAC::alarm_ticks_ms << " t \n";
		CMAC::alarm_activate(&alarm_handler, timeout);
	}

	static void set_sleeping_period(){
		unsigned short active_time = 0x0001 << _superframe_order;
		active_time *= 15;
		unsigned short total_time = 0x0001 << _beacon_order;
		total_time *= 15;
		CMAC::_sleeping_period = total_time - active_time;
		//clock drift error compensation
		if(!_coordinator){
			//unsigned short aux = (0x0001 << _superframe_order); // 2^SO
			CMAC::_sleeping_period -= 7 * _beacon_sequence_n; //the beacon transmit time is 7 ms
		}
	}

	static bool _coordinator;
	//static bool _first_execution;
	static unsigned char _beacon_order;
	static unsigned char _superframe_order;
	static unsigned char _beacon_sequence_n;

};*/

class IEEE802154_Beacon_Sync {
public:
	enum{
		MAX_BEACON_ORDER 			= 8, //up to 14
		MAX_SUPERFRAME_ORDER 		= 8,//up to 14

		DEFAULT_BEACON_ORDER	 	= 7,
		DEFAULT_SUPERFRAME_ORDER	= 4,
	};

public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Setting timeout\n";

		if(Traits<CMAC>::COORDINATOR){
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (7 << 5));//all leds on = coordinator
			//send beacons
			_beacon_order = DEFAULT_BEACON_ORDER;
			_superframe_order = DEFAULT_SUPERFRAME_ORDER;
			set_time_out();
			set_sleeping_period();
			_beacon_sequence_n = 0;
			send_beacon();
			send_beacon();
			send_beacon();
			send_beacon();
			//send_beacon(); highly unstable
		}
		else{
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (2 << 5));//led yellow = trying to sync

			//receive beacons
			set_time_out();
			CMAC::CMAC_STATE_TRANSITION result = receive_beacon();

			if(result == CMAC::TIMEOUT) {

				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
				CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (1 << 5));//led red = lost sync

				int consecutive_failures = 0;
				while(result != CMAC::RX_END){
					set_time_out();
					result = receive_beacon();

					//GAMBI - radio HW bug handling - GAMBI
					if((result == CMAC::TIMEOUT) || (result == CMAC::RX_ERROR)){
						++consecutive_failures;
						if(consecutive_failures >= 5){
							db<CMAC>(WRN) << "CMAC::IEEE802154_Beacon_Sync - Operation failed 5 times in a row, reseting radio\n";
							consecutive_failures = 0;
							CMAC::radio.hardware_reset();
							CMAC::radio.forceValidState();
						}
					}
					else{
						consecutive_failures = 0;
					}
				}
			}

			set_time_out();
			set_sleeping_period();
			db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Synchronized with coordinator\n";

			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) & ~(0xE0));
			CPU::out8(Machine::IO::PORTB, CPU::in8(Machine::IO::PORTB) | (4 << 5));//leds green = synchronized

		}


		if(CMAC::_rx_pending){
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - RX_PENDING\n";
			return CMAC::RX_PENDING;
		}
		else if(CMAC::_tx_pending){
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - TX_PENDING\n";
			return CMAC::TX_PENDING;
		}
		else{
			db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - SYNC_END\n";
			return CMAC::SYNC_END;
		}
	}

private:

	static CMAC::CMAC_STATE_TRANSITION pack_beacon(CMAC::CMAC_STATE_TRANSITION previous){
		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Creating frame\n";

		IEEE802154_Frame::beacon_frame_t *beacon_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::beacon_frame_t*>(CMAC::_frame_buffer);

		CMAC::_frame_buffer_size = sizeof(IEEE802154_Frame::beacon_frame_t);

		beacon_frame_ptr->frame_control.frameType = IEEE802154_Frame::FRAME_TYPE_BEACON;
		beacon_frame_ptr->frame_control.intraPan = IEEE802154_Frame::INTRA_PAN_SAME_PAN;
		beacon_frame_ptr->frame_control.destinationAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;
		beacon_frame_ptr->frame_control.sourceAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;

		beacon_frame_ptr->beacon_sequence_n = ++_beacon_sequence_n;
		beacon_frame_ptr->source_address = CMAC::_addr;
		beacon_frame_ptr->beacon_order = _beacon_order;
		beacon_frame_ptr->superframe_order = _superframe_order;

		beacon_frame_ptr->frame_check_sequence =
				CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Frame created:\n"
					  << *beacon_frame_ptr;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - PACK_OK\n";

		return CMAC::PACK_OK;
	}

	static CMAC::CMAC_STATE_TRANSITION send_beacon(){
		CMAC::CMAC_STATE_TRANSITION result = CMAC::TX_PENDING;

		result = pack_beacon(result);
		result = CMAC_States::Generic_Tx::execute(result);

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - TX_END\n";
		return result;
	}

	static CMAC::CMAC_STATE_TRANSITION unpack_beacon(CMAC::CMAC_STATE_TRANSITION previous){

		if(CMAC::_frame_buffer_size == 0){
			db<CMAC>(ERR) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_FAILED - Frame size == 0\n";
			return CMAC::UNPACK_FAILED;
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - Decoding frame\n";

		IEEE802154_Frame::beacon_frame_t *beacon_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::beacon_frame_t*>(CMAC::_frame_buffer);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - Frame decoded:\n"
					  << *beacon_frame_ptr;

		unsigned short crc = CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		if(beacon_frame_ptr->frame_check_sequence != crc){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - CRC error: " << crc << "\n";
			return CMAC::UNPACK_FAILED;
		}

		if(beacon_frame_ptr->frame_control.frameType != IEEE802154_Frame::FRAME_TYPE_BEACON){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_FAILED - Incorrect frame type\n";
			return CMAC::UNPACK_FAILED;
		}

		_beacon_sequence_n = beacon_frame_ptr->beacon_sequence_n;
		_beacon_order = beacon_frame_ptr->beacon_order;
		_superframe_order = beacon_frame_ptr->superframe_order;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - UNPACK_OK\n";

		return CMAC::UNPACK_OK;

	}

	static CMAC::CMAC_STATE_TRANSITION receive_beacon(){
		CMAC::CMAC_STATE_TRANSITION result = CMAC::RX_PENDING;

		while (result != CMAC::UNPACK_OK) {
			result = CMAC_States::Generic_Lpl::execute(result);
			if (result == CMAC::TIMEOUT) {
				db<CMAC>(WRN) << "CMAC_States::IEEE802154_Beacon_Sync - TIMEOUT\n";
				return CMAC::TIMEOUT;
			}
			result = CMAC_States::Generic_Rx::execute(result);
			result = unpack_beacon(result);
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Beacon_Sync - RX_END\n";
		return CMAC::RX_END;

	}

	static void alarm_handler(){
		db<CMAC>(INF) << "CMAC_States::IEEE802154_Beacon_Sync - timeout\n";
		while(!CMAC::timeout) CMAC::timeout = true;//no excuses now
		//kout << CMAC::alarm_event_time_ms << "\n";
		//CMAC::alarm_deactivate();
	}

	static void set_time_out(){
		//set the active cycle in ms
		unsigned short timeout = 0x0001 << _superframe_order;
		timeout *= 15;
		CMAC::timeout = false;
		CMAC::alarm_activate(&alarm_handler, timeout);
	}

	static void set_sleeping_period(){
		unsigned short active_time = 0x0001 << _superframe_order;
		active_time *= 15;
		unsigned short total_time = 0x0001 << _beacon_order;
		total_time *= 15;
		CMAC::_sleeping_period = total_time - active_time;
		//clock drift error compensation
		if(!Traits<CMAC>::COORDINATOR){
			//unsigned short aux = (0x0001 << _superframe_order); // 2^SO
			CMAC::_sleeping_period -= 7 * _beacon_sequence_n; //the beacon transmit time is 7 ms
		}
	}

	//static bool _coordinator;
	//static bool _first_execution;
	static unsigned char _beacon_order;
	static unsigned char _superframe_order;
	static unsigned char _beacon_sequence_n;

};

class IEEE802154_Pack {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Pack - Creating frame\n";

		IEEE802154_Frame::data_frame_header_t *header_ptr =
				reinterpret_cast<IEEE802154_Frame::data_frame_header_t*>(CMAC::_frame_buffer);

		unsigned char *payload_ptr = &(CMAC::_frame_buffer[sizeof(IEEE802154_Frame::data_frame_header_t)]);

		unsigned short *crc_ptr =
				reinterpret_cast<unsigned short*>
		(&(CMAC::_frame_buffer[sizeof(IEEE802154_Frame::data_frame_header_t)+CMAC::_tx_data_size]));

		CMAC::_frame_buffer_size = sizeof(IEEE802154_Frame::data_frame_header_t) + CMAC::_tx_data_size + 2;

		header_ptr->frame_control.frameType = IEEE802154_Frame::FRAME_TYPE_DATA;
		header_ptr->frame_control.intraPan = IEEE802154_Frame::INTRA_PAN_SAME_PAN;
		header_ptr->frame_control.destinationAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;
		header_ptr->frame_control.sourceAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;

		header_ptr->source_address = CMAC::_addr;
		header_ptr->destination_address = CMAC::_tx_dst_address;
		CMAC::_data_sequence_number = (CMAC::_data_sequence_number < 255) ? (CMAC::_data_sequence_number+1) : 0;
		header_ptr->data_sequence_n = CMAC::_data_sequence_number;

		const unsigned char *aux = reinterpret_cast<const unsigned char*>(CMAC::_tx_data);
		for (unsigned int i = 0; i < CMAC::_tx_data_size; ++i) {
			payload_ptr[i] = aux[i];
		}

		*crc_ptr = CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Pack - Frame created:\n"
				      << *header_ptr
				      << "payload_size: " << CMAC::_tx_data_size << "\n"
				      << "CRC: " << *crc_ptr << "\n";

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Pack - PACK_OK\n";

		CMAC::_transmission_count += 1;

		CMAC::_stats.total_tx_packets += 1;

		return CMAC::PACK_OK;
	}
};

class IEEE802154_Unpack {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		if(CMAC::_frame_buffer_size == 0){
			db<CMAC>(ERR) << "CMAC_States::IEEE802154_Unpack - UNPACK_FAILED - Frame size == 0\n";
			return CMAC::UNPACK_FAILED;
		}


		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Unpack - Decoding frame\n";

		IEEE802154_Frame::data_frame_header_t *header_ptr =
				reinterpret_cast<IEEE802154_Frame::data_frame_header_t*>(CMAC::_frame_buffer);

		unsigned char *payload_ptr = &(CMAC::_frame_buffer[sizeof(IEEE802154_Frame::data_frame_header_t)]);

		unsigned short *crc_ptr =
				reinterpret_cast<unsigned short*>(&(CMAC::_frame_buffer[CMAC::_frame_buffer_size - 2]));

		CMAC::_rx_data_size = CMAC::_frame_buffer_size - sizeof(IEEE802154_Frame::data_frame_header_t) - 2;//16bit crc


		db<CMAC>(INF) << "CMAC_States::IEEE802154_Unpack - Frame decoded:\n"
					  << *header_ptr
					  << "payload_size: " << CMAC::_rx_data_size << "\n"
					  << "CRC: " << *crc_ptr << "\n";


		unsigned short crc = CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		if(*crc_ptr != crc){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Unpack - CRC error: " << crc << "\n";
			CMAC::_stats.dropped_packets += 1;
			return CMAC::UNPACK_FAILED;
		}


		if(header_ptr->frame_control.frameType != IEEE802154_Frame::FRAME_TYPE_DATA){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Unpack - UNPACK_FAILED - Incorrect frame type\n";
			return CMAC::UNPACK_FAILED;
		}

		//if((header_ptr->frame_control.destinationAddressingMode != IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS) ||
		//		(header_ptr->frame_control.sourceAddressingMode != IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS)){
		//	db<CMAC>(WRN) << "CMAC_States::IEEE802154_Unpack - UNPACK_FAILED - Incorrect addressing mode\n";
		//	return CMAC::UNPACK_FAILED;
		//}


		if(header_ptr->destination_address != CMAC::_addr){
			db<CMAC>(INF) << "CMAC_States::IEEE802154_Unpack - UNPACK_FAILED - Wrong address\n";
			return CMAC::UNPACK_FAILED;
		}

		CMAC::_data_sequence_number = header_ptr->data_sequence_n;

		unsigned char *aux = reinterpret_cast<unsigned char*>(CMAC::_rx_data);
		for (unsigned int i = 0; i < CMAC::_rx_data_size; ++i) {
			aux[i] = payload_ptr[i];
		}
		CMAC::_rx_src_address = header_ptr->source_address;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Unpack - UNPACK_OK\n";

		return CMAC::UNPACK_OK;
	}
};

class IEEE802154_Ack_Rx {
public:
	static const unsigned long ACK_TIMEOUT = 30;//the RX path doesn't take more then 30 ms
	static const int MAX_TRANSMISSION_COUNT = 4;//the RX path doesn't take more then 30 ms

public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		CMAC::CMAC_STATE_TRANSITION result = CMAC::RX_PENDING;

		while (result != CMAC::UNPACK_OK) {
			result = CMAC_States::Generic_Lpl::execute(result, ACK_TIMEOUT);
			if (result == CMAC::TIMEOUT) {
				db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - Timeout\n";
				if(CMAC::_transmission_count < MAX_TRANSMISSION_COUNT){
					if(CMAC::timeout){
						db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - TIMEOUT\n";
						return CMAC::TIMEOUT;
					}
					else{
						db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - Retransmitting\n";
						return CMAC::TX_PENDING;
					}
				}
				else{
					db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - TX_FAILED\n";
					CMAC::_transmission_count = 0;
					return CMAC::TX_FAILED;
				}
			}
			result = CMAC_States::Generic_Rx::execute(result);
			result = unpack_ack(result);
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Rx - TX_OK\n";
		CMAC::_transmission_count = 0;
		return CMAC::TX_OK;
	}
private:
	static CMAC::CMAC_STATE_TRANSITION unpack_ack(CMAC::CMAC_STATE_TRANSITION input){

		if(CMAC::_frame_buffer_size == 0){
			db<CMAC>(ERR) << "CMAC_States::IEEE802154_Ack_Rx - UNPACK_FAILED - Frame size == 0\n";
			return CMAC::UNPACK_FAILED;
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Rx - Decoding frame\n";

		IEEE802154_Frame::ack_frame_t *ack_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::ack_frame_t*>(CMAC::_frame_buffer);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Ack_Rx - Frame decoded:\n"
					  << *ack_frame_ptr;

		unsigned short crc = CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		if(ack_frame_ptr->frame_check_sequence != crc){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - CRC error: " << crc << "\n";
			return CMAC::UNPACK_FAILED;
		}


		if(ack_frame_ptr->frame_control.frameType != IEEE802154_Frame::FRAME_TYPE_ACK){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - UNPACK_FAILED - Incorrect frame type\n";
			return CMAC::UNPACK_FAILED;
		}

		if(ack_frame_ptr->data_sequence_n != CMAC::_data_sequence_number){
			db<CMAC>(WRN) << "CMAC_States::IEEE802154_Ack_Rx - UNPACK_FAILED - Incorrect date sequence number\n";
			return CMAC::UNPACK_FAILED;
		}

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Rx - UNPACK_OK\n";

		return CMAC::UNPACK_OK;
	}
};

class IEEE802154_Ack_Tx {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		CMAC::CMAC_STATE_TRANSITION result = CMAC::TX_PENDING;

		result = pack_ack(result);
		result = CMAC_States::Generic_Tx::execute(result);

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Tx - RX_OK\n";
		return CMAC::RX_OK;
	}
private:
	static CMAC::CMAC_STATE_TRANSITION pack_ack(CMAC::CMAC_STATE_TRANSITION input){

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Tx - Creating frame\n";

		IEEE802154_Frame::ack_frame_t *ack_frame_ptr =
				reinterpret_cast<IEEE802154_Frame::ack_frame_t*>(CMAC::_frame_buffer);

		CMAC::_frame_buffer_size = sizeof(IEEE802154_Frame::ack_frame_t);


		ack_frame_ptr->frame_control.frameType = IEEE802154_Frame::FRAME_TYPE_ACK;
		ack_frame_ptr->frame_control.intraPan = IEEE802154_Frame::INTRA_PAN_SAME_PAN;
		ack_frame_ptr->frame_control.destinationAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;
		ack_frame_ptr->frame_control.sourceAddressingMode = IEEE802154_Frame::ADDRESSING_MODE_SHORT_ADDRESS;

		ack_frame_ptr->data_sequence_n = CMAC::_data_sequence_number;

		ack_frame_ptr->frame_check_sequence =
				CRC::crc16(reinterpret_cast<char*>(CMAC::_frame_buffer), CMAC::_frame_buffer_size - 2);

		db<CMAC>(INF) << "CMAC_States::IEEE802154_Ack_Tx - Frame created:\n"
					  << *ack_frame_ptr;

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Ack_Tx - PACK_OK\n";

		return CMAC::PACK_OK;
	}
};

class IEEE802154_Slotted_CSMA_Contention {
public:
	static CMAC::CMAC_STATE_TRANSITION execute(CMAC::CMAC_STATE_TRANSITION input) {

		db<CMAC>(TRC) << "CMAC_States::IEEE802154_Slotted_CSMA_Contention - Checking channel\n";

		unsigned int nb = 0;
		unsigned int be = MIN_BE;
		while (nb < MAX_BACKOFFS) {

			//delay = random(2^BE -1)*Period in ms
			unsigned long delay = static_cast<unsigned long> ((Pseudo_Random::random() % (2 << (be-1))) * UNIT_BACKOFF_PERIOD);

			//for some reason the delay can NEVER be 0
			if(delay == 0) delay = static_cast<unsigned long>(UNIT_BACKOFF_PERIOD);

			if(CMAC::timeout || (delay >= CMAC::alarm_event_time_left()))
				return CMAC::TIMEOUT;

			db<CMAC>(INF) << "CMAC_States::IEEE802154_Slotted_CSMA_Contention - Delay = " << delay << "\n";

			CMAC::alarm_busy_delay(delay);

			//Clear Channel Assesment(CCA)
			bool aux = false;
			CMAC::radio.CCA_measurement(aux);
			if (aux) {
				db<CMAC>(TRC) << "CMAC_States::IEEE802154_Slotted_CSMA_Contention - CHANNEL_IDLE\n";
				return CMAC::CHANNEL_IDLE;
			}
			nb = nb + 1;
			be = be + 1;
			if (be > MAX_BE)
				be = MAX_BE;
		}

		db<CMAC>(WRN) << "CMAC_States::IEEE802154_Slotted_CSMA_Contention - CHANNEL_BUSY\n";
		return CMAC::CHANNEL_BUSY;
	}

private:
	enum{
		MIN_BE = 4,
		MAX_BE = 7,
		MAX_BACKOFFS = 8,
		UNIT_BACKOFF_PERIOD = 30 //ms
	};
};


};

__END_SYS

#endif

