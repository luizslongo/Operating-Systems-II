// EPOS IP Protocol Test Program

#include <utility/ostream.h>
#include <alarm.h>
#include <nic.h>
#include <ip.h>
#include <udp.h>
#include <communicator.h>

using namespace EPOS;

const int ITERATIONS = 1;

OStream cout;
//NIC nic;
//
//int nic_test()
//{
//    NIC::Address src, dst;
//    NIC::Protocol prot;
//    char data[nic.mtu()];
//
//    NIC::Address mac = nic.address();
//    cout << "  MAC: " << mac << endl;
//
//    if(mac[5] % 2) { // sender
//        for(int i = 0; i < ITERATIONS; i++) {
//            memset(data, '0' + i, nic.mtu());
//            data[nic.mtu() - 1] = '\n';
//            nic.send(nic.broadcast(), 0x8888, data, nic.mtu());
//            cout << "  Data: " << data;
//        }
//    } else { // receiver
//        for(int i = 0; i < ITERATIONS; i++) {
//           nic.receive(&src, &prot, data, nic.mtu());
//           cout << "  Data: " << data;
//        }
//    }
//
//    NIC::Statistics stat = nic.statistics();
//    cout << "Statistics\n"
//         << "Tx Packets: " << stat.tx_packets << "\n"
//         << "Tx Bytes:   " << stat.tx_bytes << "\n"
//         << "Rx Packets: " << stat.rx_packets << "\n"
//         << "Rx Bytes:   " << stat.rx_bytes << "\n";
//
//    return stat.tx_bytes + stat.rx_bytes;
//}

int link_test()
{
    char data[20000];
    Link<UDP> * comm;

    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "  MAC: " << ip->nic()->address() << endl;


    if(ip->address()[3] % 2) { // sender
        cout << "Sender:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]--;

        comm = new Link<UDP>(8000, Link<UDP>::Address(peer_ip, UDP::Port(8000)));

        for(int i = 0; i < ITERATIONS; i++) {
            data[0] = '\n';
            data[1] = ' ';
            data[2] = '0' + i;
            data[3] = '0' + i;
            data[4] = '0' + i;
            data[5] = '0' + i;
            data[6] = '0' + i;
            data[7] = '0' + i;

            for(int j = 8; j < sizeof(data) - 8; j += 8) {
                data[j+0] = ' ';
                data[j+1] = '0' + (j / 1000000 % 10);
                data[j+2] = '0' + (j / 100000 % 10);
                data[j+3] = '0' + (j / 10000 % 10);
                data[j+4] = '0' + (j / 1000 % 10);
                data[j+5] = '0' + (j / 100 % 10);
                data[j+6] = '0' + (j / 10 % 10);
                data[j+7] = '0' + (j % 10);
            }

            data[sizeof(data) - 8] = ' ';
            data[sizeof(data) - 7] = '0' + i;
            data[sizeof(data) - 6] = '0' + i;
            data[sizeof(data) - 5] = '0' + i;
            data[sizeof(data) - 4] = '0' + i;
            data[sizeof(data) - 3] = '0' + i;
            data[sizeof(data) - 2] = '\n';
            data[sizeof(data) - 1] = 0;

            int sent = comm->send(&data, sizeof(data));
            if(sent == sizeof(data))
                cout << "  Data: " << data << endl;
            else
                cout << "  Data was not correctly sent. It was " << sizeof(data) << ", but only " << sent << "bytes were sent!"<< endl;
        }
    } else { // receiver
        cout << "Receiver:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]++;

        comm = new Link<UDP>(8000, Link<UDP>::Address(peer_ip, UDP::Port(8000)));

        for(int i = 0; i < ITERATIONS; i++) {
            comm->receive(&data, sizeof(data));
            cout << "  Data: " << data << endl;
        }
    }

    NIC::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << "\n";

    return stat.tx_bytes + stat.rx_bytes;
}

int main()
{
    Network::init();

    cout << "IP Test Program" << endl;
    cout << "Sizes:" << endl;
    cout << "  NIC::Header => " << sizeof(NIC::Header) << endl;
    cout << "  IP::Header => " << sizeof(IP::Header) << endl;
    cout << "  UDP::Header => " << sizeof(UDP::Header) << endl;

    link_test();

    return 0;
}
