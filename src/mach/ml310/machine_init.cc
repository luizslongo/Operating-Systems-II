// EPOS-- ML310 Mediator Initialization

#include <mach/ml310/machine.h>

__BEGIN_SYS

int ML310::init(System_Info * si)
{
    db<ML310>(TRC) << "ML310::init()\n";

    // Set all interrupt handlers to int_not()
    for(unsigned int i = 0; i < INT_VECTOR_SIZE; i++)
       int_vector(i, int_not);

    //Install Processor Exception Handlers
    int_vector(ML310_IC::INT_PROGRAM, exc_program);

    return 0;
}

__END_SYS
