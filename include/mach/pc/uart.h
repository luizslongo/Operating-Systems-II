// EPOS-- PC UART Mediator

#ifndef __pc_uart_h
#define __pc_uart_h

#include <uart.h>

__BEGIN_SYS

class PC_UART: protected UART_Common
{
private:
    typedef Traits<PC_UART> Traits;
    static const Type_Id TYPE = Type<PC_UART>::TYPE;

public:
    PC_UART();
    PC_UART(Baud_Rate baudrate, Data_Bits databits, Parity parity, Stop_Bits stopbits);
    ~PC_UART();

    int set_baud_rate(Baud_Rate baudrate);
    int set_data_bits(Data_Bits databits);
    int set_parity(Parity parity);
    int set_stop_bits(Stop_Bits stopbits);
    int receive_byte(unsigned char *rec_char);
    int send_byte(unsigned char sen_char);
    int line_empty();
    int data_ready();

    static int init(System_Info * si);
};

typedef PC_UART UART;

__END_SYS

#endif
