// EPOS-- ATMega16_Display Test Program

#include <utility/ostream.h>
#include <display.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "ATMega16_Display test\n";

    ATMega16_Display display;

    return 0;
}
