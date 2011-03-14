// EPOS-- MC13224v Radio (Transceiver) NIC Mediator Implementation

#include <utility/malloc.h>
#include <mach/mc13224v/machine.h>
#include <mach/mc13224v/radio.h>
#include <mach/mc13224v/buck_regulator.h>

__BEGIN_SYS

MC13224V_Transceiver * Radio_Wrapper::device = new MC13224V_Transceiver();

void Radio_Wrapper::init() {
    MC13224V_Transceiver::maca_init();

    device->set_channel(0); /* 802.15.4 channel 11 */
    device->set_power(0x12); /* 4.5dBm */

}

void Radio_Wrapper::set_event_handler(MC13224V_Transceiver::event_handler * handler) {
    device->set_event_handler(handler);
}

int Radio_Wrapper::send(unsigned char * data, unsigned int size) {
    volatile MC13224V_Transceiver::packet_t * p;

    do {
        p = device->get_free_packet();
    }while (!p);

    device->fill_packet(p, data, size);
    device->tx_packet(p);

    return size;
}

int Radio_Wrapper::receive(unsigned char * data) {
    volatile MC13224V_Transceiver::packet_t * p;
    do {
        device->check_maca();

        p = device->rx_packet();
    }while (!p);

    for (int i = 0; i < p->length; i++) {
        data[i] = p->data[i + 1];
    }
    int size = p->length;
    device->free_packet(p);

    return size;
}

void Radio_Wrapper::off() {
    device->reset_maca();
}

void Radio_Wrapper::listen() {
    MC13224V_Transceiver::event_handler * ev = device->get_event_handler();
    ev(MC13224V_Transceiver::SFD_DETECTED);
}

void Radio_Wrapper::reset() {
    device->reset_maca();
}

bool Radio_Wrapper::cca() {
//    return device->cca_measurement(MC13224V_Transceiver::ENERGY_ABOVE_THRESHOLD, 0);
    return false;
}

__END_SYS

