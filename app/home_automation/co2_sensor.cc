#include <modbus_ascii.h>
#include <active.h>
#include <nic.h>
#include <mach/mc13224v/cm1101.h>
#include <battery.h>

__USING_SYS

NIC * nic;
OStream cout;

class Receiver : public Modbus_ASCII::Modbus_ASCII_Feeder,
                  public Active
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
      : Modbus_ASCII(sender, addr), _sensor(0), _battery(0) {}

	virtual ~Modbus() {}

	void set_sensor(CM1101 * sensor)
	{
		_sensor = sensor;
	}

	void set_battery(Battery * battery)
	{
		_battery = battery;
	}

	virtual void handle_command(unsigned char cmd, unsigned char * data, int data_len)
	{
		switch(cmd)
		{
		case READ_HOLDING_REGISTER:
			read(data[1]);
			break;
		// cases for other commands: writes to config?
		}
	}
private:
	void read(unsigned char reg)
	{
		union
		{
			float f;
			unsigned char uc[sizeof(float)];
		} response;

		switch(reg)
		{
		case 0x00:
			response.f = _sensor->co2();
			break;
		case 0x04:
			response.f = _sensor->temperature();
			break;
		case 0x08:
			response.f = _sensor->humidity();
			break;
		case 0x0c:
			response.f = _battery->charge();
		}

		send(myAddress(), READ_HOLDING_REGISTER, reg, response.uc, sizeof(float));
	}

	CM1101 * _sensor;
	Battery * _battery;
};

int main()
{
	cout << "CM1101 CO2 Sensor" << endl;

	nic = new NIC();
    UART sensor_uart(1);
    CM1101 sensor(&sensor_uart);

    Receiver receiver(nic);
    Sender sender(nic);
    Modbus modbus(&sender, 0xA1);
    modbus.set_sensor(&sensor);
    modbus.set_battery(Battery::sys_batt());
    receiver.registerModbus(&modbus);

    receiver.start();

//    receiver.notify(":A12300C4\r\n\0");
//    receiver.notify(":A12304C8\r\n\0");
//    receiver.notify(":A12308CC\r\n\0");
//    receiver.notify(":A1230CD0\r\n\0");

    receiver.join();

    return 0;
}
