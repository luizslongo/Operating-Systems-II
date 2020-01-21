// EPOS Network Common Package

#ifndef __network_common_h
#define __network_common_h

#include <system/config.h>

extern "C" { void __pre_main(); }

__BEGIN_SYS

class Network_Common
{
     friend void ::__pre_main();

private:
    static void init();
};

__END_SYS

#endif
