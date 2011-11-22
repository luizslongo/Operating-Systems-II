// EPOS PC Network Abstraction Test

#include <system/config.h>

#ifdef __NIC_H

#include <network.h>

__USING_SYS

OStream cout;

int main()
{
    Network net;
    Network::Address to(10,0,1,1);
    Network::Address from;
    Network::Protocol prot;
    char data[1500];

    for(int i = 0; i < 10; i++)
	net.send(to, "alguem ai?\n", 12);
    for(int i = 0; i < 10; i++) 
	net.receive(&from, data, 1500);

    Network::Statistics stat = net.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
};

#else

int main() { return 0; }

#endif

