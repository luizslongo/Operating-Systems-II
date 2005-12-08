// EPOS-- ATMega16 NIC Mediator Test

#include <mach/atmega16/nic.h>

__USING_SYS

OStream cout;

int main()
{
    ATMega16_NIC net;
    ATMega16_NIC::Address src, dst;
    ATMega16_NIC::Protocol prot;
    char data[ATMega16_NIC::MTU];

    for(int i = 0; i < 10; i++)
	net.send(ATMega16_NIC::BROADCAST, 0x8888, "alguem ai?\n", 12);
    for(int i = 0; i < 10; i++) 
	net.receive(&src, &prot, data, ATMega16_NIC::MTU);

    ATMega16_NIC::Statistics stat = net.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
}
