#include <smart_data.h>
#include <alarm.h>
#include <thread.h>

using namespace EPOS;

OStream cout;

void sensor(const NIC::Address & mac);
void sink(const NIC::Address & mac);

int main()
{
    Network::init();

    cout << "TSTP test" << endl;
    cout << "Configuration:" << endl;
    cout << "  Size of TSTP::Header:      " << sizeof(TSTP::Header) << endl;
    cout << "  Size of TSTP::Packet:      " << sizeof(TSTP::Packet) << endl;
    cout << "  Size of TSTP::Interest:    " << sizeof(TSTP::Interest) << endl;
    cout << "  Size of TSTP::Response:    " << sizeof(TSTP::Response) << endl;
    cout << "  Size of TSTP::Command:     " << sizeof(TSTP::Command) << endl;
    cout << "  Size of TSTP::Unit:        " << sizeof(TSTP::Unit) << endl;
    cout << "  Size of TSTP::Value<I32>:  " << sizeof(TSTP::Value<TSTP::Unit::I28>) << endl;
    cout << "  Size of TSTP::Value<I60>:  " << sizeof(TSTP::Value<TSTP::Unit::I60>) << endl;
    cout << "  Size of TSTP::Value<F32>:  " << sizeof(TSTP::Value<TSTP::Unit::F32>) << endl;
    cout << "  Size of TSTP::Value<D64>:  " << sizeof(TSTP::Value<TSTP::Unit::D64>) << endl;
    cout << "  Size of TSTP::Time_Offset: " << sizeof(TSTP::Time_Offset) << endl;
    cout << "  Size of TSTP::Time:        " << sizeof(TSTP::Time) << endl;
    cout << "  Size of TSTP::Microsecond: " << sizeof(TSTP::Microsecond) << endl;
    cout << "  Size of TSTP::Coordinates: " << sizeof(TSTP::Coordinates) << endl;
    cout << "  Size of TSTP::Region:      " << sizeof(TSTP::Region) << endl;

    NIC nic;
    NIC::Address mac = nic.address();

    if(mac[5] % 2)
        sink(mac);
    else
        sensor(mac);

    cout << "I'm done, bye!" << endl;

    return 0;
}

void sink(const NIC::Address & mac)
{
    cout << "I'm the sink!" << endl;
    cout << "I'm running on a machine whose MAC address is " << mac << "." << endl;
    cout << "My location is " << TSTP::here() << " and the time now is " << TSTP::now() << endl;
    cout << "I'll now declare my interests ..." << endl;

    Acceleration a0(Region(Coordinates(5, 5, 5), 10, 0, 3000000000UL), 1000000); // Remote, from a region centered at (1, 1, 1), with radius 10, from time 20 to 30, updated on each event, with expiration time of 1s.
//    Delay (10000000);
//    Acceleration a1(Region(Coordinates(5, 5, 5), 10, 0, 3000000000UL), 1000000, 1000000); // Remote, from a region centered at (1, 1, 1), with radius 10, from time 20 to 30, updated every ten seconds, with expiration time of 1s.


    do {
        cout << "a0=" << a0 << endl;
//        cout << "a1=" << a1 << endl;
        Delay (10000000);
    } while((a0 != 'a') && (a0 != 'a'));
}

void sensor(const NIC::Address & mac)
{
    cout << "I'm the sensor!" << endl;
    cout << "I'm running on a machine whose MAC address is " << mac << "." << endl;
    cout << "My location is " << TSTP::here() << " and the time now is " << TSTP::now() << endl;
    cout << "I'll now create sensors ..." << endl;

    Acceleration a0(0, 1000000); // Local, private, with expiration time of 1 s.
    Acceleration a1(1, 10000000, Acceleration::COMMANDED); // Local, commanded, with local expiration time of 10 s.

    while(1)
        Delay(50000);
}

