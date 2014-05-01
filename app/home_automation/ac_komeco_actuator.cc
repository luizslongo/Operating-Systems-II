#include <system/config.h>
#include <modbus_ascii.h>
#include <active.h>
#include <nic.h>
#include "ac_komeco_control.h"

__USING_SYS

NIC * nic;
OStream cout;

const char AC_Komeco_Control::_temperature[15] = {
	Degrees17,
	Degrees18,
	Degrees19,
	Degrees20,
	Degrees21,
	Degrees22,
	Degrees23,
	Degrees24,
	Degrees25,
	Degrees26,
	Degrees27,
	Degrees28,
	Degrees29,
	Degrees30,
	FanMode
};

class Receiver : public Modbus_ASCII::Modbus_ASCII_Feeder
{
public:
	Receiver(NIC * n) : _nic(n) {}
	virtual ~Receiver() {}

	virtual int run()
	{
	    NIC::Protocol prot;
	    NIC::Address src;

	    char msg[Modbus_ASCII::MSG_LEN];

		while(true)
		{
	        while(!(nic->receive(&src, &prot, &msg, Modbus_ASCII::MSG_LEN) > 0));
	        Modbus_ASCII::Modbus_ASCII_Feeder::notify(msg);
		}
		return 0;
	}

	// to debug only
	void notify(const char * c)
	{
		Modbus_ASCII::Modbus_ASCII_Feeder::notify(c);
	}

private:
	NIC * _nic;
};

class Sender : public Modbus_ASCII::Modbus_ASCII_Sender
{
public:
	Sender(NIC * n) : _nic(n) {}
	virtual ~Sender() {}

	virtual void send(char * c, int len)
	{
		memcpy(_msg, c, len);
		memset(_msg+len, 0x00, Modbus_ASCII::MSG_LEN-len);

        int r;
        while ((r = nic->send(NIC::BROADCAST, (NIC::Protocol) 1, c, Modbus_ASCII::MSG_LEN)) != 11)
            cout << "Send fail: " << r << "\n";

	}

private:
	NIC * _nic;
	char _msg[Modbus_ASCII::MSG_LEN];
};

class Modbus : public Modbus_ASCII
{
public:
	Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr), _temp(18)
     {}

	virtual ~Modbus() {}

	virtual void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		kout << "received command: " << hex << (int)cmd;
		for (int i = 0; i < data_len; ++i)
			kout << " " << (int)data[i];
		kout << dec << endl;
		unsigned short coil, value;
		switch(cmd)
		{
		case WRITE_SINGLE_COIL:
			coil = (((unsigned short)data[0]) << 8) | data[1];
			value = data[2];
			if(coil == 0x00) _ac.ac_onoff(value);
			break;
//		case WRITE_MULTIPLE_COILS:
//			for(int i = 0; i < data[0]; ++i)
//				???
//			break;
		case WRITE_REGISTER:
			switch(data[0])
			{
			case 0x00:
				value = data[1];
				_ac.ac_temperature(value);
				break;
			case 0x01:
				//TODO: adjust fan speed
				break;
			}
			break;
		}
	}
private:
	AC_Komeco_Control _ac;
	int _temp;
};

int main()
{
	cout << "AC Controller." << endl;

	nic = new NIC();

    Receiver receiver(nic);
    Sender sender(nic);
    Modbus modbus(&sender, 0xA7);
    receiver.registerModbus(&modbus);
    receiver.run();

    return 0;
}
