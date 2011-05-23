// EPOS ATMega1281 Radio (AT86RF230) NIC Mediator Declarations

#ifndef __at86rf230_nic_h
#define __at86rf230_nic_h

#include "at86rf230.h"

__BEGIN_SYS

class ATMega1281_Radio
{
public:
    static const int FRAME_BUFFER_SIZE = AT86RF230::FRAME_MAX_LENGTH;

    enum Event {
	SFD_DETECTED,
	FRAME_END
    };

    typedef AT86RF230::Event Event;
    typedef unsigned char Address;

    ATMega1281_Radio() {}
    ~ATMega1281_Radio() {}

    static void init();

    static void set_event_handler(AT86RF230::event_handler * handler);

    int send(unsigned char * data, unsigned int size);

    int receive(unsigned char * data);

    void on();

    void off();

    void sleep();

    void listen();

    void reset();

    bool cca();

private:
    static AT86RF230 * device;
};

__END_SYS

#endif

