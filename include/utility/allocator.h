// EPOS-- Allocator Utility Declarations

#ifndef __allocator_h
#define	__allocator_h

#include <system/config.h>
#include <utility/list.h>

__BEGIN_SYS

enum {
    INCREMENTAL,
    AOFF_NC,
    AOFF_C,
    BINARY_BUDDY
};

template<int algorithm = INCREMENTAL>
class Allocator {};

__END_SYS

#endif
