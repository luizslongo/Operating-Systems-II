#include <modbus_ascii.h>
#include <active.h>
#include <nic.h>
#include <gpio_pin.h>
#include <periodic_thread.h>

#include <alarm.h>

__USING_SYS

NIC * nic;
OStream cout;

class Receiver : public Modbus_ASCII::Modbus_ASCII_Feeder
{
public:
	Receiver(NIC * n) : _nic(n) {}
	virtual ~Receiver() {}

	virtual int run()
	{
	    NIC::Protocol prot;
	    NIC::Address src;

		while(true)
		{
	        while(!(nic->receive(&src, &prot, &_msg, Modbus_ASCII::MSG_LEN) > 0));
	        Modbus_ASCII::Modbus_ASCII_Feeder::notify(_msg);
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
    char _msg[Modbus_ASCII::MSG_LEN];
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
private:
	enum {
		LAMP0_PIN = 12,
		LAMP1_PIN = 13
//		LAMP0_PIN = 8,
//		LAMP1_PIN = 9
	};

public:
	Modbus(Modbus_ASCII_Sender * sender, unsigned char addr)
      : Modbus_ASCII(sender, addr)
     {
		 _temperature_sensor = new Temperature_Sensor();
		_gpio[0] = new GPIO_Pin(LAMP0_PIN);
		_gpio[1] = new GPIO_Pin(LAMP1_PIN);
     }

	virtual ~Modbus() {}

	virtual void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		kout << "received command: " << hex << (int)cmd;
		for (int i = 0; i < data_len; ++i)
			kout << (int)data[i];
		kout << dec;
		unsigned short coil, value;
		switch(cmd)
		{
		case WRITE_SINGLE_COIL:
			coil = (((unsigned short)data[0]) << 8) | data[1];
			value = (((unsigned short)data[2]) << 8) | data[3];
			write(coil, value);
			break;
		case READ_HOLDING_REGISTER:
			read(data[1]);
			break;
//		case WRITE_MULTIPLE_COILS:
//			for(int i = 0; i < data[0]; ++i)
//				write(data[1+i*2], data[2+i*2]);
//			break;
//		case READ_MULTIPLE_REGISTERS:
//			send(myAddress(), READ_MULTIPLE_REGISTERS, _state, 2);
//			break;
		}
	}
private:
	void write(unsigned short output, unsigned short value)
	{
		kout << "write: " << output << " , " << value << endl;
		if(value) _gpio[output]->set();
		else _gpio[output]->clear();
	}

	void read(unsigned char reg)
	{
		union
		{
			float f;
			unsigned char uc[sizeof(float)];
		} response;
		
		bool ok = false;

		switch(reg)
		{
//		case 0x00:
//			response.f = _sensor->co2();
//			break;
		case 0x04:
			response.f = _temperature_sensor->sample();
			ok = true;
			break;
//		case 0x08:
//			response.f = _sensor->humidity();
//			break;
//		case 0x0c:
//			response.f = _battery->charge();
		case 0x10:
			response.f = _temperature_sensor->sample();
			ok = true;
			break;
		}
		if(ok)
			send(myAddress(), READ_HOLDING_REGISTER, reg, response.uc, sizeof(float));
	}

	GPIO_Pin * _gpio[2];
	char _state[2];
    Temperature_Sensor * _temperature_sensor;
};

int main()
{
	cout << "Lamp actuator" << endl;

	nic = new NIC();

    Receiver receiver(nic);
    Sender sender(nic);
    Modbus modbus(&sender, 0xA6);
    receiver.registerModbus(&modbus);
	receiver.run();

    return 0;
}
