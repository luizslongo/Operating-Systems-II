// EPOS-- Segment Initialization

#include <segment.h>

__BEGIN_SYS

int Segment::init(System_Info * si)
{
    db<Segment>(TRC) << "Segment::init()\n";

    return 0;
}

__END_SYS
