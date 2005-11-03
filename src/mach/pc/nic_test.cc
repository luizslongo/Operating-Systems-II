// EPOS-- PC NIC Mediator Test

#include <mach/pc/nic.h>

__USING_SYS

OStream cout;

int main()
{
    PC_NIC net;
    PC_NIC::Address src, dst;
    PC_NIC::Protocol prot;
    char data[PC_NIC::MTU];

    for(int i = 0; i < 10; i++)
	net.send(PC_NIC::BROADCAST, 0x8888, "alguem ai?\n", 12);
    for(int i = 0; i < 10; i++) 
	net.receive(&src, &prot, data, PC_NIC::MTU);

    PC_NIC::Statistics stat = net.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
}
