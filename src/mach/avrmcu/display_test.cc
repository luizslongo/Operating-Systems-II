// EPOS-- AVRMCU_Display Test Program

#include <utility/ostream.h>
#include <display.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "AT90S_Display test\n";

    AVRMCU_Display display;

    return 0;
}
