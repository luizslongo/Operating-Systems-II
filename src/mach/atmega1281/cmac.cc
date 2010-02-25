// EPOS-- CMAC Implementation
#include <machine.h>
#include <mach/atmega1281/nic.h>
#include <mach/atmega1281/transceiver.h>
#include <mach/atmega1281/cmac.h>
#include <semaphore.h>

__BEGIN_SYS

OStream print;

template <class Sync, 
	  class Pack, class Contention, class Tx, class Ack_Rx,
	  class Lpl, class Rx, class Unpack, class Ack_Tx> 
CMAC::CMAC_STATE_TRANSITION CMAC::state_machine() {

	CMAC_STATE_TRANSITION result = TIMER_INT;
	_state = SYNC;

	db<CMAC>(TRC) << "CMAC::state_machine - starting state machine\n";

	//unsigned long gambi_latency = alarm_ticks_ms;//TODO remove this

	while (_state != OFF) {
		switch (_state) {
		case SYNC:
			result = Sync::execute(result);
			if(result == SYNC_END)
				_state = OFF;
			else if(result == RX_PENDING)
				_state = LPL;
			else if(result == TX_PENDING)
				_state = PACK;
			break;
		case PACK: //first TX state
			if (Traits<CMAC>::TIME_TRIGGERED)
				_last_sm_exec_tx = true;
			result = Pack::execute(result);
			if(result == PACK_OK)
				_state = CONTENTION;
			else if(result == PACK_FAILED)
				_state = OFF;
			break;
		case CONTENTION:
			result = Contention::execute(result);
			if(result == CHANNEL_IDLE)
				_state = TX;
			else if((result == CHANNEL_BUSY) || (result == TIMEOUT))
				_state = OFF;
			break;
		case TX:
			result = Tx::execute(result);
			if(result == TX_END)
				_state = ACK_RX;
			else if(result == TX_ERROR)
				_state = OFF;
			break;
		case ACK_RX:
			result = Ack_Rx::execute(result);
			if(result == TX_PENDING)
				_state = PACK;
			else
				_state = OFF;
			break;
		case LPL: //first RX state
			if (Traits<CMAC>::TIME_TRIGGERED)
				_last_sm_exec_rx = true;
			result = Lpl::execute(result);
			if(result == TIMEOUT)
				_state = OFF;
			else if(result == PREAMBLE_DETECTED)
				_state = RX;
			break;
		case RX:
			result = Rx::execute(result);
			if(result == RX_END)
				_state = UNPACK;
			else if(result == RX_ERROR)
				_state = OFF;
			break;
		case UNPACK:
			result = Unpack::execute(result);
			if(result == UNPACK_FAILED)
				_state = LPL;
			else if(result == UNPACK_OK)
				_state = ACK_TX;
			break;
		case ACK_TX:
			result = Ack_Tx::execute(result);
			_state = OFF;
			break;
		case OFF:
			break;
		}
	}

	//TODO remove this
	//if(_tx_pending || _rx_pending){
	//	kout << alarm_ticks_ms - gambi_latency << "\n";
	//}

	//GAMBI - radio HW bug handling - GAMBI
	if((result == TIMEOUT) || (result == TX_FAILED) || (result == CHANNEL_BUSY) ||
	   (result == TX_ERROR) || (result == RX_ERROR)){
		++_consecutive_failures;
		if(_consecutive_failures >= 5){
			db<CMAC>(WRN) << "CMAC::state_machine - Operation failed 5 times in a row, reseting radio\n";
			_consecutive_failures = 0;
			radio.hardware_reset();
			radio.forceValidState();
		}
	}
	else{
		_consecutive_failures = 0;
	}


	db<CMAC>(TRC) << "CMAC::state_machine - state machine finished executing\n";

	return result;
}

int CMAC::send(const Address & dst, const Protocol & prot,
		const void *data, unsigned int size) {

	if (Traits<CMAC>::TIME_TRIGGERED){
		if(_tx_pending || (size > mtu())){
			db<CMAC>(WRN) << "CMAC::send - another TX pending or data size > MTU\n";
			return -1;
		}
	}
	else{
		if(_tx_pending || _rx_pending || (size > mtu())){
			db<CMAC>(WRN) << "CMAC::send - another TX or RX pending or data size > MTU\n";
			return -1;
		}
	}

	if(size == 0){
		db<CMAC>(ERR) << "CMAC::send - data size = 0\n";
		return 0;
	}

	CMAC_STATE_TRANSITION result;

	_tx_data = data;
	_tx_data_size = size;
	_tx_dst_address = dst;
	_tx_pending = true;

	unsigned long start_time = alarm_ticks_ms;

	if (Traits<CMAC>::TIME_TRIGGERED){
		db<CMAC>(INF) << "CMAC::send - waiting for TX handling\n";
		_sem_tx.p();
		result = _state_machine_result;
	}
	else{
		db<CMAC>(TRC) << "CMAC::send - calling state machine\n";
		result = state_machine<
				Traits<CMAC>::Sync_State,
				Traits<CMAC>::Pack_State,
				Traits<CMAC>::Contention_State,
				Traits<CMAC>::Tx_State,
				Traits<CMAC>::Ack_Rx_State,
				Traits<CMAC>::Lpl_State,
				Traits<CMAC>::Rx_State,
				Traits<CMAC>::Unpack_State,
				Traits<CMAC>::Ack_Tx_State
				>();
	}

	if(result == TX_OK){
		_stats.tx_packets += 1;
		_stats.tx_bytes += size;
	}
	_stats.tx_time += alarm_ticks_ms - start_time;

	_tx_pending = false;

	return result;
}

int CMAC::receive(Address * src, Protocol * prot,
		void * data, unsigned int size) {

	if (Traits<CMAC>::TIME_TRIGGERED){
		if(_rx_pending || (size > mtu())){
			db<CMAC>(WRN) << "CMAC::receive - another RX pending or buffer size > MTU\n";
			return -1;
		}
	}
	else{
		if(_rx_pending || _tx_pending || (size > mtu())){
			db<CMAC>(WRN) << "CMAC::receive - another RX or TX pending or buffer size > MTU\n";
			return -1;
		}
	}

	CMAC_STATE_TRANSITION result;

	_rx_data = data;
	_rx_data_size = size;
	_rx_pending = true;

	unsigned long start_time = alarm_ticks_ms;

	if (Traits<CMAC>::TIME_TRIGGERED){
		db<CMAC>(INF) << "CMAC::receive - waiting for RX handling\n";
		_sem_rx.p();
		result = _state_machine_result;
	}
	else{
		db<CMAC>(TRC) << "CMAC::receive - calling state machine\n";
		result = state_machine<
				Traits<CMAC>::Sync_State,
				Traits<CMAC>::Pack_State,
				Traits<CMAC>::Contention_State,
				Traits<CMAC>::Tx_State,
				Traits<CMAC>::Ack_Rx_State,
				Traits<CMAC>::Lpl_State,
				Traits<CMAC>::Rx_State,
				Traits<CMAC>::Unpack_State,
				Traits<CMAC>::Ack_Tx_State
				>();
	}

	if(result == RX_OK){
		_stats.rx_packets += 1;
		_stats.rx_bytes += _rx_data_size;
	}
	_stats.rx_time += alarm_ticks_ms - start_time;

	*src = _rx_src_address;
	*prot = 0;

	_rx_pending = false;

	if(result == RX_OK)
		return _rx_data_size;
	else
		return result*(-1);
}

void CMAC::state_machine_handler(){
	if (Traits<CMAC>::TIME_TRIGGERED){

		//alarm_deactivate();
		_on_active_cycle = true;

		_state_machine_result =
				state_machine<
				Traits<CMAC>::Sync_State,
				Traits<CMAC>::Pack_State,
				Traits<CMAC>::Contention_State,
				Traits<CMAC>::Tx_State,
				Traits<CMAC>::Ack_Rx_State,
				Traits<CMAC>::Lpl_State,
				Traits<CMAC>::Rx_State,
				Traits<CMAC>::Unpack_State,
				Traits<CMAC>::Ack_Tx_State
				>();

		//kills the rest of the active cycle
		if(Traits<CMAC>::TIMEOUT != 0){
			while(!timeout);
		}

		alarm_activate(&(CMAC::state_machine_handler), _sleeping_period);

		//in the case of a timeout, try again next time
		//if(_state_machine_result != TIMEOUT){
			if(_last_sm_exec_tx){
				_last_sm_exec_tx = false;
				_sem_tx.v();
			}
			else if(_last_sm_exec_rx){
				_last_sm_exec_rx = false;
				_sem_rx.v();
			}
		//}
	}
}

/*
void CMAC::alarm_handler_int_handler(unsigned int){
	CPU::int_disable();
	timer_int.v();
	CPU::int_enable();
}

int CMAC::alarm_thread_entry(){
	while(true){
		timer_int.p();
		alarm_handler_function(0);
	}
	return 0;
}
*/

void CMAC::alarm_handler_function(){

	CPU::int_disable();

	//increment in 2 ms //see cmac_init
	//alarm_ticks_ms = (alarm_ticks_ms >= 0xFFFFFFFC) ? 0 : (alarm_ticks_ms + 2);
	alarm_ticks_ms += 2;

	CPU::int_enable();

	//db<CMAC>(INF) << alarm_ticks_ms << "\n";

	if((alarm_ev_handler != 0) && (alarm_ticks_ms >= alarm_event_time_ms)){
		db<CMAC>(INF) << "CMAC::alarm_handler_function - calling alarm_ev_handler\n";
		event_handler *tmp = alarm_ev_handler;
		alarm_ev_handler = 0;
		(tmp());
	}
}

void CMAC::sm_step_int_handler(){
	sm_step_next_step = true;
}

volatile bool CMAC::sm_step_next_step = false;


Transceiver CMAC::radio;

CMAC::Address CMAC::_addr = Traits<CMAC>::ADDRESS;

volatile CMAC::CMAC_STATE CMAC::_state = CMAC::OFF;
volatile CMAC::Statistics CMAC::_stats;

//used only when TIME_TRIGGERED = true
Semaphore CMAC::_sem_rx(0);
Semaphore CMAC::_sem_tx(0);
CMAC::CMAC_STATE_TRANSITION CMAC::_state_machine_result = CMAC::UNPACK_FAILED;
volatile bool CMAC::_last_sm_exec_tx = false;
volatile bool CMAC::_last_sm_exec_rx = false;
volatile bool CMAC::_on_active_cycle = false;

unsigned char CMAC::_frame_buffer[FRAME_BUFFER_SIZE];
unsigned int CMAC::_frame_buffer_size = 0;

void* CMAC::_rx_data = 0;
const void* CMAC::_tx_data = 0;
unsigned int CMAC::_rx_data_size = 0;
unsigned int CMAC::_tx_data_size = 0;
CMAC::Address CMAC::_tx_dst_address = 0;
CMAC::Address CMAC::_rx_src_address = 0;
volatile bool CMAC::_rx_pending = false;
volatile bool CMAC::_tx_pending = false;
int CMAC::_transmission_count = 0;

unsigned char CMAC::_data_sequence_number = 0;


//CMAC states static variables
volatile bool CMAC::timeout = false;
//Semaphore CMAC_States::Generic_Lpl::listen_sem(0);
volatile bool CMAC_States::Generic_Lpl::_frame_received = false;

//IEEE802.15.4 specific variables
//bool CMAC_States::IEEE802154_Beacon_Sync::_coordinator = false;
//bool CMAC_States::IEEE802154_Beacon_Sync::_first_execution = true;
unsigned char CMAC_States::IEEE802154_Beacon_Sync::_beacon_order =  CMAC_States::IEEE802154_Beacon_Sync::MAX_BEACON_ORDER;
unsigned char CMAC_States::IEEE802154_Beacon_Sync::_superframe_order = CMAC_States::IEEE802154_Beacon_Sync::MAX_SUPERFRAME_ORDER;
unsigned char CMAC_States::IEEE802154_Beacon_Sync::_beacon_sequence_n = 0;

int CMAC::_consecutive_failures = 0;

__END_SYS

