// EPOS-- ATMega128 NIC Mediator Test

#include <mach/atmega128/nic.h>

__USING_SYS

OStream cout;

int main()
{
    ATMega128_NIC net;
    ATMega128_NIC::Address src, dst;
    ATMega128_NIC::Protocol prot;
    char data[ATMega128_NIC::MTU];

    for(int i = 0; i < 10; i++)
	net.send(ATMega128_NIC::BROADCAST, 0x8888, "alguem ai?\n", 12);
    for(int i = 0; i < 10; i++) 
	net.receive(&src, &prot, data, ATMega128_NIC::MTU);

    ATMega128_NIC::Statistics stat = net.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
}
