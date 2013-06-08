// EPOS Address_Space Abstraction Implementation

#include <address_space.h>

__BEGIN_SYS

// Methods
Address_Space::Address_Space()
{
    db <Address_Space> (TRC) << "Address_Space() {Directory::pd=" << Directory::pd() << "}" << endl;
}


Address_Space::Address_Space(const Self & s)
: Directory(reinterpret_cast<MMU::Page_Table *>(CPU::pdp()))
{
    db <Address_Space> (TRC) << "Address_Space(SELF) => {Directory::pd=" << reinterpret_cast<void *>(CPU::pdp()) << "}" << endl;
}


Address_Space::~Address_Space()
{
    db<Address_Space>(TRC) << "~Address_Space() {Directory::pd=" << Directory::pd() << "}" << endl;
}


Address_Space::Log_Addr Address_Space::attach(const Segment & seg)
{
    Log_Addr tmp = Directory::attach(seg);

    db <Address_Space> (TRC) << "Address_Space::attach(seg=" << &seg << ") => " << tmp << endl;

    return tmp;
}


Address_Space::Log_Addr Address_Space::attach(const Segment & seg, Address_Space::Log_Addr addr)
{
    Log_Addr tmp = Directory::attach(seg, addr);

    db <Address_Space> (TRC) << "Address_Space::attach(seg=" << &seg << ",addr=" << addr << ") => " << tmp << endl;

    return tmp;
}


void Address_Space::detach(const Segment & seg)
{
    db <Address_Space> (TRC) << "Address_Space::detach(seg=" << &seg << ")" << endl;

    Directory::detach(seg);
}


Address_Space::Phy_Addr Address_Space::physical(Address_Space::Log_Addr address)
{
    return Directory::physical(address);
}

__END_SYS
