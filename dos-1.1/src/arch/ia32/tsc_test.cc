// EPOS IA32_TSC Test Program

#include <utility/ostream.h>
#include <tsc.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "IA32_TSC test\n";

    IA32_TSC tsc;

    return 0;
}
