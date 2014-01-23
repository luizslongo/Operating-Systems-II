// EPOS CM1101 Sensor Mediator Declarations

#ifndef __cm1101_h
#define __cm1101_h

#include <uart.h>

__BEGIN_SYS

class CM1101 {
public:
	enum Status {
		OK = 0x00,
		BAD_LEN = 0x01,
		BAD_CMD = 0x02,
		BAD_STT = 0x03,
		BAD_CRC = 0x04
	};

	CM1101(UART * u)
		: _uart(u), _firmware_version(0), _co2(0),
		  _temperature(0), _humidity(0), _status(OK)
	{
		check_firmware_version();
	}

	~CM1101() {
		if(!_firmware_version) kfree(_firmware_version);
	}

	char * firmware_version() {
		return _firmware_version;
	}

	unsigned int serial_number(); //FIXME: results doesn't match protocol specification (contact supplier)

	float co2() {
		update_data();
		return _co2;
	}

	float temperature() {
		update_data();
		return _temperature;
	}

	float humidity() {
		update_data();
		return _humidity;
	}

	char status() {
		return _status;
	}

    int get() { return co2(); }

    int sample() {
    	return co2();
    }

    bool enable() { return false; /*return _uart.enable(); */}
    void disable() { /*_uart.disable(); */ }
    bool data_ready() { return _uart->has_data(); }

protected:
	void check_firmware_version() {
		send_command(0x1E);
		char ck = _uart->get(); // ACK/NACK
		if(ck == 0x16) { // ACK
			char len = _uart->get(); // length
			_firmware_version = (char*)kmalloc(len-1);
			char cmd = _uart->get();
			char cs = ck + len + cmd;
			for(int i = 0; i < (len-1); i++) {
				_firmware_version[i] = _uart->get();
				cs += _firmware_version[i];
			}
			char c = _uart->get();
			if(c == (0x100 - cs)) {
				_status = OK;
				return; // SUCCESS
			}
		}
		// ELSE
		_firmware_version = (char*)kmalloc(5);
		if(ck == 0x16) { // BAD CS
			_status = BAD_CRC;
			memcpy(_firmware_version, "CSerr", 5);
		}
		else { // NAK
			char len = _uart->get();
			char cmd = _uart->get();
			char err = _uart->get();
			char cs = _uart->get();
			if(cs == (0x100 - (ck+len+cmd+err))) {
				_status = (Status)err;
				memcpy(_firmware_version, "NACK", 4);
				_firmware_version[4] = err;
			}
			else {
				_status = BAD_CRC;
				memcpy(_firmware_version, "CSerr", 5);
			}
		}
	}

	void update_data() {
		send_command(0x01);
		char ck = _uart->get(); // ACK/NACK
		if(ck == 0x06) { // NACK
			char len = _uart->get();
			char cmd = _uart->get();
			char err = _uart->get();
			char cs = _uart->get();
			if(cs == (0x100 - (ck+len+cmd+err))) _status = (Status)err;
			else _status = BAD_CRC;
		}
		// ACK!
		char len = _uart->get(); // length
		char cmd = _uart->get(); // CMD
		char * resp = (char*)kmalloc(len-1);
		char cs = ck + len + cmd;
		for(int i = 0; i < (len-1); i++) {
			resp[i] = _uart->get();
			cs += resp[i];
		}
		char c = _uart->get();
		if(c != (0x100 - cs)) { // FAIL
			_status = BAD_CRC;
			kfree(resp);
			return;
		}
		_co2 = resp[0]*0x100+resp[1];
		_temperature = ((resp[2]*0x100+resp[3])/10.0 - 32) / 1.8;
		_humidity = (resp[4]*0x100+resp[5])/10.0;
		_status = OK;
		kfree(resp);
	}

	void send_command(unsigned char cmd, unsigned char * df = 0, unsigned int df_len = 0) {
		_uart->put(0x11);
		_uart->put(df_len+1);
		_uart->put(cmd);
		unsigned char df_sum = 0;
		for(unsigned int i = 0; i < df_len; i++) {
			_uart->put(df[i]);
			df_sum += df[i];
		}
		_uart->put(0x100-(0x11+(df_len+1)+cmd+df_sum));
	}

	UART * _uart;
	char * _firmware_version;
	float _co2;
	float _temperature;
	float _humidity;
	char _status;
};

__END_SYS

#endif
