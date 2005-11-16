// EPOS-- ATMega128_Display Test Program

#include <utility/ostream.h>
#include <display.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "ATMega128_Display test\n";

    ATMega128_Display display;

    return 0;
}
