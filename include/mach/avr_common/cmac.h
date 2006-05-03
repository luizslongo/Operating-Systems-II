#ifndef __cmac_h
#define __cmac_h

#include "cc1000/cc1000.h"
#include <nic.h>
#include <cpu.h>
#include <utility/crc.h>

__BEGIN_SYS 

class CMAC: public Low_Power_Radio {

private:

    //Display disp;
    enum STATE { IDLE, SYNC, RECV };

    static const int FREQUENCY = Traits<CMAC>::FREQUENCY;
    static const int POWER = Traits<CMAC>::POWER;

public:
  
    CMAC(int unit = 0) {
	_cc1000.config(FREQUENCY,POWER);
	_addr = _cc1000.id();
    }


    int send(const Address & dst, const Protocol & prot,
	     const void *data, unsigned int size) {
	
	for(unsigned int i = 0; i < 0xff; i++);  // backoff lol

	_cc1000.enable();
			 
	Frame f(dst, _cc1000.id(), prot, data, size);
	f._tx_pow = 0x00;
	f._rss = 0x00;
			 
	f._crc = CRC::crc16((char *) &f, sizeof(Frame) - 2);
			 
	unsigned char *fp = (unsigned char *) &f;

	_cc1000.tx_mode();

	CPU::int_disable();

	for (int i = 0; i < 18; i++)
	    _cc1000.put(~0x55);

	_cc1000.put(~0xCC);
	_cc1000.put(~0x33);

	for (unsigned int i = 0; i < sizeof(Frame); i++) {
		
	    _cc1000.put(~*fp);
	    fp++;
		
	}

	_cc1000.put(~0x55);

	CPU::int_enable();

	_stats.tx_bytes += sizeof(Frame);
	_stats.tx_packets++;
	
	_cc1000.disable();

	return size;
	
    }

    int receive(Address * src, Protocol * prot, 
		void * data, unsigned int size) {

	_cc1000.enable();
			
	STATE state = IDLE;
	unsigned char rx_buf_lsb = 0;
	unsigned char rx_buf_msb = 0;
	unsigned char preamble_count = 0;
	unsigned char rx_bit_offset = 0;
	unsigned char sync_count = 0;
	unsigned char recv_bytes = 0;

	_cc1000.rx_mode();
	
	while (1) {
	
	    unsigned char d = _cc1000.get();

	    switch (state) {
	    case IDLE:
		if ((d == 0xAA) || (d == 0x55)) {
		    preamble_count++;
		    if (preamble_count > 2) {
			preamble_count = sync_count = 0;
			rx_bit_offset = 0;
			state = SYNC;
		    }
		} else {
		    preamble_count = 0;
		}
		break;
	    case SYNC:
		CPU::int_disable();
		if ((d == 0xAA) || (d == 0x55)) {
		    rx_buf_msb = d;
		} else {
		    switch (sync_count) {
		    case 0:
			rx_buf_lsb = d;
			break;
						
		    case 1:
		    case 2:
			unsigned int tmp = (rx_buf_msb << 8) | rx_buf_lsb;
			rx_buf_msb = rx_buf_lsb;
			rx_buf_lsb = d;
							
			for (int i = 0; i < 8; i++) {
			    tmp <<= 1;					
			    if (d & 0x80)
				tmp |= 0x01;				
			    d <<= 1;
			    if (tmp == 0xCC33) {
				state = RECV;
				rx_bit_offset = 7 - i;
				break;
			    }				
			}
			break;
						
		    default:
			preamble_count = 0;
			state = IDLE;
			break;
					
		    }
		    sync_count++;
		}
		break;
				
	    case RECV:
	
		rx_buf_msb = rx_buf_lsb;
		rx_buf_lsb = d;
		
		charbuf[recv_bytes] = (0x00ff & ((rx_buf_msb << 8) | rx_buf_lsb) >> rx_bit_offset);
		recv_bytes++;		
		
		if (recv_bytes == sizeof(Frame)) {

		    CPU::int_enable();

		    _cc1000.disable();
			
		    Frame * frame = (Frame *) charbuf;	
			
		    unsigned short crc = CRC::crc16((char *) frame, sizeof(Frame) - 2);
		
		    _stats.rx_bytes += sizeof(Frame);
		    _stats.rx_packets++;
			
		    if(crc != frame->_crc) {
			_stats.dropped_packets++;
			return 0;
		    }	
			
		    *src = frame->_src;
		    *prot = frame->_prot;
	    
		    memcpy(data, frame->_data, size);
			
		    return size;
			
		}
	
	    }
	
	}
	
	return 0;

    }
    const Address & address() {
	return _addr;
    }
	
    const Statistics & statistics() {
	return _stats;
    }

    void reset() { 
	_cc1000.config(FREQUENCY,POWER);
    }

    void config(int frequency, int power){
	_cc1000.config(frequency,power);
    }

    unsigned int mtu() const { return MTU; }

    static void init(unsigned int n);

private:
    CC1000 _cc1000;
    Address _addr;
    Statistics _stats;
    unsigned char charbuf[sizeof(Frame)];

};

__END_SYS
#endif
