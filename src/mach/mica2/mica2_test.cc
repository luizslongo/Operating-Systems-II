// EPOS-- AVRMCU Test Program

#include <utility/ostream.h>
#include <machine.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "Mica2 test\n";

    Mica2 machine;

    return 0;
}
