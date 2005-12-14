// EPOS-- PC_Timer Test Program

#include <utility/ostream.h>
#include <timer.h>

__USING_SYS

int main()
{
    OStream cout;

    cout << "PC_Timer test\n";

    PC_Timer timer(1000);
    
    cout << "count = " << timer.read() << "\n";
    cout << "count = " << timer.read() << "\n";
    cout << "count = " << timer.read() << "\n";
    cout << "count = " << timer.read() << "\n";
    cout << "count = " << timer.read() << "\n";
    cout << "count = " << timer.read() << "\n";
    
    cout << "The End!\n";

    return 0;
}
