// EPOS-- Address_Space Abstraction Declarations

#ifndef __address_space_h
#define __address_space_h

#include <system/config.h>
#include <mmu.h>
#include <segment.h>

__BEGIN_SYS

class Address_Space: public MMU::Directory
{
private:
    typedef Traits<Address_Space> Traits;
    static const Type_Id TYPE = Type<Address_Space>::TYPE;

    typedef CPU::Phy_Addr Phy_Addr;
    typedef CPU::Log_Addr Log_Addr;
    typedef MMU::Directory Directory;

public:
    Address_Space() {
	db<Address_Space>(TRC) << "Address_Space() [Directory::pd=" 
			       << Directory::pd() << "]\n";
    }
    Address_Space(MMU::Page_Directory * pd) : Directory(pd) {
	db<Address_Space>(TRC) << "Address_Space(pd=" << pd << "\n";
    }
    ~Address_Space() {
	db<Address_Space>(TRC) << "~Address_Space() [Directory::pd=" 
			       << Directory::pd() << "]\n";
    }

   Log_Addr attach(const Segment & seg) {
	db<Address_Space>(TRC) << "Address_Space::attach(seg=" << &seg
			       << ")\n";

	return Directory::attach(seg);
    }
    Log_Addr attach(const Segment & seg, Log_Addr addr) {
	db<Address_Space>(TRC) << "Address_Space::attach(seg=" << &seg
			       << ",addr=" << addr << ")\n";

	return Directory::attach(seg, addr);
    }
    void detach(const Segment & seg) {
	db<Address_Space>(TRC) << "Address_Space::detach(seg=" << &seg 
			       << ")\n";

	Directory::detach(seg);
    }

    void activate() {
	db<Address_Space>(TRC) 
	    << "Address_Space::activate() [Directory::pd=" 
	    << Directory::pd() << "]\n";

	Directory::activate();
    }

    Phy_Addr physical(Log_Addr address) { 
	return Directory::physical(address);
    }

    static int init(System_Info * si);
};

__END_SYS

#endif
