//OpenEPOS MC13224V Default Application

#include <alarm.h>
#include <machine.h>
#include <nic.h>
#include <uart.h>

__USING_SYS

const unsigned char SINK_ID = 0x01;
const unsigned int DATA_SIZE = 64;

NIC * nic;
OStream cout;

char msg[DATA_SIZE];

void set_led()
{
    ;
}

void clear_led()
{
    ;
}

int sensor(unsigned char id) {

    cout << "sensor id = " << id << "\n";

    set_led();

    for (int z = 0; z < 30; z++) {

        nic->send(SINK_ID, (NIC::Protocol) 0, &msg, sizeof(msg));

        Alarm::delay(200000);
    }

    NIC::Statistics stats = nic->statistics();

    clear_led();

    cout << stats << "\n";

    return 0;
}

void button_int() {
    cout << "Statistics:\n";
    cout << nic->statistics() << "\n";
}

int sink() {
    cout << "Sink\n";

    IC::int_vector(IC::IRQ_KBI7, button_int);
    IC::enable(IC::IRQ_KBI7);

    NIC::Protocol prot;
    NIC::Address src;

    int num = 0;
    while (num < 30) {
        nic->receive(&src, &prot, &msg, sizeof(msg));
        num++;
    }

    return 0;
}

int main() {
    nic = new NIC;
    sensor(2);
//    sensor(3);
//    sink();

    return 0;
}
