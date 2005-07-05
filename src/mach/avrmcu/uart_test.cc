// EPOS AVRMCU_UART Test Program

//#include <utility/ostream.h>
#include <uart.h>


__USING_SYS

int main()
{
    OStream cout;

    cout << "AVRMCU_UART test\n";

    AVRMCU_UART uart;

    return 0;

}
