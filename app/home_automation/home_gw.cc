#include <modbus_ascii.h>
#include <active.h>
#include <nic.h>
#include <uart.h>

__USING_SYS

NIC *nic;
UART uart;

class Receiver : public Active
{
public:
	Receiver() {}
	virtual ~Receiver() {}

	virtual int run()
	{
		NIC::Protocol prot;
	    NIC::Address src;

	    int i = 0;
		while(true)
		{
			memset(_msg, 0x00, Modbus_ASCII::MSG_LEN);
	        while(!(nic->receive(&src, &prot, &_msg, Modbus_ASCII::MSG_LEN) > 0))
	        	yield();
	        i = 0;
	        while(!(_msg[i] == '\r' && _msg[i+1] == '\n')) {
	        	uart.put(_msg[i++]);
	        }
	        uart.put('\r');
	        uart.put('\n');
		}
		return 0;
	}

private:
    unsigned char _msg[Modbus_ASCII::MSG_LEN];
};

class Sender : public Active
{
public:
	Sender() {}
	virtual ~Sender() {}

	virtual int run()
	{
	    int i;
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

    Receiver receiver;
    Sender sender;

    receiver.start();
    sender.start();

    receiver.join();
    sender.join();

    kerr << "All threads finished! Something is wrong!" << endl;

    return 0;
}
