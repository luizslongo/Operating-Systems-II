// EPOS-- PC PCI Mediator

#ifndef __pc_pci_h
#define __pc_pci_h

#include <pci.h>

__BEGIN_SYS

class PC_PCI: public PCI_Common
{
private:
    typedef Traits<PC_PCI> Traits;
    static const Type_Id TYPE = Type<PC_PCI>::TYPE;

    static const int MAX_BUS = Traits::MAX_BUS;
    static const int MAX_DEV_FN = Traits::MAX_DEV_FN;
    static const unsigned long LOG_IO_MEM = Memory_Map<PC>::IO_MEM;

    // From Intel 82443BX manual
    // CONFADDR is a 32 bit register accessed only when referenced as a Dword.
    // A Byte or Word reference will "pass through" the Configuration Address
    // Register onto the PCI bus as an I/O cycle. The CONFADDR register
    // contains the Bus Number, Device Number, Function Number, and Register
    // Number for which a subsequent configuration access is intended.
    // [31]    -> config enable
    // [24:30] -> reserved
    // [11:15] -> device (5 bits)
    // [ 8:10] -> function (3 bits)
    // [ 2: 7] -> register (6 bits)
    // [ 0: 1] -> reserved
    static const int CONFADDR = 0xcf8;

    // CONFDATA is a 32 bit read/write window into configuration space.
    // The portion of configuration space that is referenced by CONFDATA is
    // determined by the contents of CONFADDR.
    static const int CONFDATA = 0xcfc;

public:
    PC_PCI() {}

    Locator scan(const Class_Id & c, int order);
    Locator scan(const Vendor_Id & v, const Device_Id & d, int order);
    
    void header(const Locator & l, Header * h);
    Reg16 command(const Locator & l) {
	return cfg16(l.bus, l.dev_fn, COMMAND);
    }
    void  command(const Locator & l, Reg16 v) {
	cfg16(l.bus, l.dev_fn, COMMAND, v);
    }
    Reg16 status(const Locator & l) {
	return cfg16(l.bus, l.dev_fn, STATUS);
    }
    void  status(const Locator & l, Reg16 v) {
	cfg16(l.bus, l.dev_fn, STATUS, v);
    }
    
    static int init(System_Info * si);

private:
    int cmd(Reg8 bus, Reg8 dev_fn, Reg8 addr) {
	return 0x80000000 | (bus << 16) | (dev_fn << 8) | (addr & ~3);
    }

    Reg8 cfg8(Reg8 bus, Reg8 dev_fn, Reg8 addr) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));
	return IA32::in8(CONFDATA + (addr & 3));
    }
    Reg16 cfg16(Reg8 bus, Reg8 dev_fn, Reg8 addr) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));    
	return IA32::in16(CONFDATA + (addr & 2));
    }
    Reg32 cfg32(Reg8 bus, Reg8 dev_fn, Reg8 addr) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));
	return IA32::in32(CONFDATA);
    }
    void cfg8(Reg8 bus, Reg8 dev_fn, Reg8 addr, Reg8 value) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));    
	IA32::out8(CONFDATA + (addr & 3), value);
    }
    void cfg16(Reg8 bus, Reg8 dev_fn, Reg8 addr, Reg16 value) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));
	IA32::out16(CONFDATA + (addr & 2), value);
    }
    void cfg32(Reg8 bus, Reg8 dev_fn, Reg8 addr, Reg32 value) {
	IA32::out32(CONFADDR, cmd(bus, dev_fn, addr));
	IA32::out32(CONFDATA, value);
    }

    Reg16 vendor_id(Reg8 bus, Reg8 dev_fn) {
	return cfg16(bus, dev_fn, VENDOR_ID); 
    }
    Reg16 device_id(Reg8 bus, Reg8 dev_fn) {
	return cfg16(bus, dev_fn, DEVICE_ID); 
    }
    Reg16 class_id(Reg8 bus, Reg8 dev_fn) {
	return cfg16(bus, dev_fn, CLASS_ID); 
    }

    Log_Addr phy2log(const Phy_Addr & addr) {
	return LOG_IO_MEM + (addr - _phy_io_mem);
    } 

  private:
    static Phy_Addr _phy_io_mem;
    static Reg32 _base_address[Region::N];
};

typedef PC_PCI PCI;

__END_SYS

#endif
