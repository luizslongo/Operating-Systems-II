#include <utility/ostream.h>

using namespace EPOS;

OStream cout;

int main()
{
    cout << "Hello world!" << endl;

    unsigned long long a = 1234;

ASM("baba:");
    cout << "a=" << a << endl;

    return 0;
}
