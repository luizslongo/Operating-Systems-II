// EPOS Address_Space Abstraction Declarations

#ifndef __address_space_h
#define __address_space_h

#include <mmu.h>
#include <segment.h>

__BEGIN_SYS

class Address_Space: private MMU::Directory
{
private:
    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::Log_Addr Log_Addr;
    typedef MMU::Directory Directory;

public:
    Address_Space();
    Address_Space(const Self & s);
    ~Address_Space();

    Log_Addr attach(const Segment & seg);
    Log_Addr attach(const Segment & seg, Log_Addr addr);
    void detach(const Segment & seg);

    using MMU::Directory::activate;
    using MMU::Directory::pd;

    Phy_Addr physical(Log_Addr address);
};

__END_SYS

#endif
