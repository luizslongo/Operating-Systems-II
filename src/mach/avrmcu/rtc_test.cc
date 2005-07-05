// EPOS-- AVRMCU_IC Test Program

#include <utility/ostream.h>
#include <rtc.h>
#include <machine.h>

__USING_SYS


int main()
{
    OStream cout;

    cout << "AVRMCU_RTC test\n";

    AVRMCU_RTC rtc;

    return 0;

}
