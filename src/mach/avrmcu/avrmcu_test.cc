// EPOS-- AVRMCU Test Program

#include <utility/ostream.h>
#include <machine.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "AVRMCU test\n";

    AVRMCU machine;

    return 0;
}
