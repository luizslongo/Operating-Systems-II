// EPOS PC_NIC Test Programs

#include <utility/ostream.h>
#include <nic.h>

using namespace EPOS;

OStream cout;

int main()
{
    NIC nic;
    NIC::Address src, dst;
    NIC::Protocol prot;
    char data[nic.mtu()];

    for(int i = 0; i < 10; i++)
//while(1)
        nic.send(NIC::BROADCAST, 0x8888, "alguem ai?\n", 12);
    return 0;
    for(int i = 0; i < 10; i++) 
	nic.receive(&src, &prot, data, nic.mtu());

    NIC::Statistics stat = nic.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
}
