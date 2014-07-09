// EPOS IP Protocol Test Program

#include <utility/ostream.h>
#include <utility/random.h>
#include <communicator.h>

using namespace EPOS;

const int ITERATIONS = 3;

OStream cout;

int icmp_test()
{
    IP * ip = IP::get_by_nic(0);

    cout << "  IP: " << ip->address() << endl;
    cout << "  MAC: " << ip->nic()->address() << endl;

    if(ip->address()[3] % 2) { // sender
        cout << "Sender:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]--;
        ICMP::Packet packet;
        Port<ICMP> * comm = new Port<ICMP>(0);
        unsigned int id = Random::random();

        for(int i = 0; i < ITERATIONS; i++) {
            new (&packet) ICMP::Packet(ICMP::ECHO, 0, id, i);

            int sent = comm->send(peer_ip, &packet, sizeof(ICMP::Packet));
            if(sent == sizeof(ICMP::Packet))
                cout << "  Data: " << &packet << endl;
            else
                cout << "  Data was not correctly sent. It was " << sizeof(ICMP::Packet) << " bytes long, but only " << sent << "bytes were sent!"<< endl;
            Delay(100000);
        }
    } else { // receiver
        cout << "Receiver:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]++;
        ICMP::Packet packet;
        Port<ICMP> * comm = new Port<ICMP>(0);

        for(int i = 0; i < ITERATIONS; i++) {
            ICMP::Address from;
            int received = comm->receive(&from, &packet, sizeof(ICMP::Packet));
            if(received == sizeof(ICMP::Packet))
                cout << "  Data: " << &packet << endl;
            else
                cout << "  Data was not correctly received. It was " << sizeof(ICMP::Packet) << " bytes long, but " << received << " bytes were received!"<< endl;

            if(packet.type() == ICMP::ECHO) {
                db<ICMP>(WRN) << "ICMP::update: echo request from " << from << endl;

                ICMP::Packet * reply = new (&packet) ICMP::Packet(ICMP::ECHO_REPLY, 0);
                comm->send(from, reply, sizeof(packet));
            } else if(packet.type() == ICMP::ECHO_REPLY)
                db<ICMP>(WRN) << "ICMP::update: echo reply to " << from << endl;
        }
    }

    NIC::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

    return stat.tx_bytes + stat.rx_bytes;
}

int udp_test()
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
                data[j+1] = '0' + i + (j / 1000000 % 10);
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
                cout << "  Data was not correctly sent. It was " << sizeof(data) << " bytes long, but only " << sent << "bytes were sent!"<< endl;
        }
    } else { // receiver
        cout << "Receiver:" << endl;

        IP::Address peer_ip = ip->address();
        peer_ip[3]++;

        comm = new Link<UDP>(8000, Link<UDP>::Address(peer_ip, UDP::Port(8000)));

        for(int i = 0; i < ITERATIONS; i++) {
            int received = comm->receive(&data, sizeof(data));
            if(received == sizeof(data))
                cout << "  Data: " << data << endl;
            else
                cout << "  Data was not correctly received. It was " << sizeof(data) << " bytes long, but " << received << " bytes were received!"<< endl;
        }
    }

    NIC::Statistics stat = ip->nic()->statistics();
    cout << "Statistics\n"
         << "Tx Packets: " << stat.tx_packets << "\n"
         << "Tx Bytes:   " << stat.tx_bytes << "\n"
         << "Rx Packets: " << stat.rx_packets << "\n"
         << "Rx Bytes:   " << stat.rx_bytes << endl;

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

    icmp_test();
    udp_test();

    return 0;
}
