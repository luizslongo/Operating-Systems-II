// EPOS assert Utility Implementation

#include <utility/assert.h>

#include <machine.h>

namespace System
{

void assert(bool exp)
{
    if (!exp)
    {
        Machine::panic();
    }
    
}


}
