#include <modbus_ascii.h>
#include <nic.h>
#include <uart.h>

__USING_SYS

NIC *nic;
UART uart;


int main()
{
	kout << "EPOSMoteII Home Automation Sniffer" << endl;

	nic = new NIC();

	NIC::Protocol prot;
	NIC::Address src;
    unsigned char _msg[Modbus_ASCII::MSG_LEN];

	int i = 0;
	while(true)
	{
		memset(_msg, 0x00, Modbus_ASCII::MSG_LEN);
		while(!(nic->receive(&src, &prot, &_msg, Modbus_ASCII::MSG_LEN) > 0));
		i = 0;
		while(!(_msg[i] == '\r' && _msg[i+1] == '\n')) {
			uart.put(_msg[i++]);
		}
		uart.put('\r');
		uart.put('\n');
	}

    return 0;
}
