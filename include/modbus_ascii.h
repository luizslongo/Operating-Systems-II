#ifndef __modbus_ascii_h
#define __modbus_ascii_h

#include <system/config.h>

__BEGIN_SYS

class Modbus_ASCII
{
private:
	static char char_map[16]; //FIXME: static initialization

public:
	static const int MSG_LEN = 96;
	enum Commands {
		READ_HOLDING_REGISTER = 0X03,
		WRITE_SINGLE_COIL = 0X05,
		WRITE_MULTIPLE_COILS = 0x15,
		WRITE_REGISTER = 0X16,
		READ_MULTIPLE_REGISTERS = 0x23
	};

	class Modbus_ASCII_Feeder
	{
	public:
		virtual ~Modbus_ASCII_Feeder() {};

		void registerModbus(Modbus_ASCII * modbus) { _modbus = modbus; }

	protected:
		void notify(const char * c)
		{
			_modbus->receive(c);
		}
		Modbus_ASCII * _modbus;
	};

	class Modbus_ASCII_Sender
	{
	public:
		virtual ~Modbus_ASCII_Sender() {}

		virtual void send(char * c, int len) = 0;
	};

	Modbus_ASCII(Modbus_ASCII_Sender * sender, unsigned char addr, int max_data_size = 32)
	  : _sender(sender), _my_address(addr)
    {
		_data_in = new unsigned char [max_data_size];
		_data_out = new char [max_data_size];
		int i = 0;
		char_map[i++] = '0'; //FIXME: static initialization
		char_map[i++] = '1';
		char_map[i++] = '2';
		char_map[i++] = '3';
		char_map[i++] = '4';
		char_map[i++] = '5';
		char_map[i++] = '6';
		char_map[i++] = '7';
		char_map[i++] = '8';
		char_map[i++] = '9';
		char_map[i++] = 'A';
		char_map[i++] = 'B';
		char_map[i++] = 'C';
		char_map[i++] = 'D';
		char_map[i++] = 'E';
		char_map[i++] = 'F';
    }

	virtual ~Modbus_ASCII()
	{
		delete [] _data_in;
		delete [] _data_out;
	}

	void receive(const char * cmd)
	{
		kout << "RECEIVED: " << hex;
		int i = 0;
		while(state_machine(cmd[i++]))
			kout << cmd[i-1];
		kout << cmd[i-1] << dec;
	}

	void send(unsigned char addr, unsigned char cmd, unsigned char reg, unsigned char * data, int data_len)
	{
		unsigned char lrc = 0;

		int i = 0;
		_data_out[i++] = ':';

		encode(addr, &_data_out[i], &_data_out[i+1]); i += 2;
		lrc += addr;

		encode(cmd, &_data_out[i], &_data_out[i+1]); i += 2;
		lrc += cmd;

		encode(0x00, &_data_out[i], &_data_out[i+1]); i += 2;
		encode(reg, &_data_out[i], &_data_out[i+1]); i += 2;
		lrc += reg;

		for(int j = 0; j < data_len; ++j)
		{
			encode(data[j], &_data_out[i], &_data_out[i+1]); i += 2;
			lrc += data[j];
		}

		lrc = ((lrc ^ 0xff) + 1) & 0xff;
		encode(lrc, &_data_out[i], &_data_out[i+1]); i += 2;

		_data_out[i++] = '\r';
		_data_out[i] = '\n';

		kout << "RESPONSE: " << hex;
		for(int j = 0; j < i; ++j)
			kout << _data_out[j];
		kout << dec << endl;

		_sender->send(_data_out, i);
	}

	static void encode(unsigned char c, char * ascii1, char * ascii2)
	{
		*ascii2 = char_map[c&0x0f];
		*ascii1 = char_map[c>>4];
	}
	static char dec_one(char c)
	{
		if(c < 0x40) return (c - 0x30);
		else return (c - 0x37);
	}
	static char decode(char h, char l)
	{
		return ((dec_one(h) << 4) | dec_one(l));
	}

	unsigned char myAddress() { return _my_address; }

	virtual void handle_command(unsigned char cmd, unsigned char * data, int data_len) = 0;

private:

	int data_index;
	unsigned char cmd;
	unsigned char data_len;
	unsigned char _lrc;
	char STATE;
	unsigned char buff;

	int state_machine(char c)
	{
		switch(STATE) {
		case 0:
			data_index = 0;
			data_len = 0;
			if(c == ':') STATE = 1;
			break;
		case 1:
			buff = c;
			STATE = 2;
			break;
		case 2:
			buff = decode(buff, c);
			if(buff == _my_address) STATE = 3;
			else STATE = 0;
			break;
		case 3:
			buff = c;
			STATE = 4;
			break;
		case 4:
			cmd = decode(buff, c);
			STATE = 5;
			break;
		case 5:
			if(c == '\r')
				STATE = 7;
			else {
				buff = c;
				STATE = 6;
			}
			break;
		case 6:
			_data_in[data_len++] = decode(buff, c);
			STATE = 5;
			break;
		case 7:
			if(c != '\n') STATE = 0;
			_lrc = _my_address + cmd;
			for(int i = 0; i < (data_len-1); ++i)
				_lrc += _data_in[i];
			_lrc = ((_lrc ^ 0xff) + 1) & 0xff;
			STATE = 0;
			if(_lrc == _data_in[data_len-1]) handle_command(cmd, _data_in, data_len-1);
			else kout << "bad lrc" << endl;
			break;
		}

		return STATE;
	}

	Modbus_ASCII_Sender * _sender;
	unsigned char _my_address;
	unsigned char * _data_in;
	char * _data_out;
};

__END_SYS

#endif

