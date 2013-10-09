// EPOS PC_NIC Test Programs

#include <utility/ostream.h>
#include <alarm.h>
#include <nic.h>

using namespace EPOS;

OStream cout;

int main()
{
    NIC nic;
    NIC::Address src, dst;
    NIC::Protocol prot;
    char data[nic.mtu()];

    NIC::Address self = nic.address();
    cout << "  MAC: " << self << endl;

    if(self[5] % 2) { // sender
        for(int i = 0; i < 10; i++) {
            nic.send(NIC::BROADCAST, 0x8888, "alguem ai?\n", 12);
            Delay delay(1000000);
        }
    } else {
        for(int i = 0; i < 10; i++) {
           nic.receive(&src, &prot, data, nic.mtu());
           cout << "  Data: " << data;
        }
    }

    NIC::Statistics stat = nic.statistics();
    cout << "Statistics\n"
	 << "Tx Packets: " << stat.tx_packets << "\n"
	 << "Tx Bytes:   " << stat.tx_bytes << "\n"
	 << "Rx Packets: " << stat.rx_packets << "\n"
	 << "Rx Bytes:   " << stat.rx_bytes << "\n";
}
