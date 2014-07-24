#include <modbus_ascii.h>
#include <active.h>
#include <nic.h>
#include <uart.h>
#include <alarm.h>

__USING_SYS

NIC *nic;
UART uart;
OStream cout;

class Sender : public Active
{
public:
	Sender() {}
	virtual ~Sender() {}

	virtual int run()
	{		
	    int i;
		i=0;
		_msg[i++] = ':';
		_msg[i++] = 'A';
		_msg[i++] = '2';
		_msg[i++] = '0';
		_msg[i++] = '5';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '1';
		_msg[i++] = 'f';
		_msg[i++] = 'f';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '5';
		_msg[i++] = '9';
		_msg[i++] = '\r';
		_msg[i++] = '\n';
		memset(_msg+i, 0x00, Modbus_ASCII::MSG_LEN-i);
		while ((nic->send(NIC::BROADCAST, (NIC::Protocol) 1, &_msg, Modbus_ASCII::MSG_LEN)) != 11)
			yield();
		Alarm::delay(1000000);
		i=0;
		_msg[i++] = ':';
		_msg[i++] = 'A';
		_msg[i++] = '2';
		_msg[i++] = '0';
		_msg[i++] = '5';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = 'f';
		_msg[i++] = 'f';
		_msg[i++] = '0';
		_msg[i++] = '0';
		_msg[i++] = '5';
		_msg[i++] = 'A';
		_msg[i++] = '\r';
		_msg[i++] = '\n';
		memset(_msg+i, 0x00, Modbus_ASCII::MSG_LEN-i);
		while ((nic->send(NIC::BROADCAST, (NIC::Protocol) 1, &_msg, Modbus_ASCII::MSG_LEN)) != 11)
			yield();

		while(true)
		{
			i = 0;
			while(!uart.has_data()) yield();
			_msg[i++] = uart.get();
			while(!uart.has_data()) yield();
			_msg[i++] = uart.get();
	        while(!(_msg[i-2] == '\r' && _msg[i-1] == '\n')) {
				while(!uart.has_data()) yield();
	        	_msg[i++] = uart.get();
	        }
			memset(_msg+i, 0x00, Modbus_ASCII::MSG_LEN-i);
	        int r;
	        while ((r = nic->send(NIC::BROADCAST, (NIC::Protocol) 1, &_msg, Modbus_ASCII::MSG_LEN)) != 11)
	            yield();
		}

		return 0;
	}

private:
    unsigned char _msg[Modbus_ASCII::MSG_LEN];
};

int main()
{
    nic = new NIC();

    Sender sender;

    sender.start();

    sender.join();

    kerr << "All threads finished! Something is wrong!" << endl;

    return 0;
}
