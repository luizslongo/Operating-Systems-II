// EPOS-- Common Declarations for AVR EEPROMs
// Author: Arliones

#ifndef __avr_eeprom_h
#define __avr_eeprom_h

#include <machine.h>
#include <cpu.h>

__BEGIN_SYS

class AVR_EEPROM
{
protected:
    static const int SIZE = 512; // bytes

    typedef IO_Map<Machine> IO;
    typedef CPU::Reg8 Reg8;
    typedef CPU::Log_Addr Log_Addr;

    //EEPROM IO Register Bit Offsets
    enum {
        //EEARH
        EEAR8 = 1,
	//EEARL
        EEAR7 = 7,
        EEAR6 = 6,
        EEAR5 = 5,
        EEAR4 = 4,
        EEAR3 = 3,
        EEAR2 = 2,
        EEAR1 = 1,
        EEAR0 = 0,
	//EECR
	EERIE = 3,
	EEMWE = 2,
	EEWE  = 1,
	EERE  = 0
    };

public:
    AVR_EEPROM() {};

    unsigned char read_byte(Log_Addr addr);
    void write_byte(Log_Addr addr, unsigned char data);
    int size() { return SIZE; }

private:

    typedef CPU::Reg8 Reg8;
    typedef CPU::Reg16 Reg16;

    Reg8 eedr() { return CPU::in8(IO::EEDR); }
    void eedr(Reg8 value) { CPU::out8(IO::EEDR,value); }
    Reg8 eecr() { return CPU::in8(IO::EECR); }
    void eecr(Reg8 value) { CPU::out8(IO::EECR,value); }
    Reg8 eearl() { return CPU::in8(IO::EEARL); }
    void eearl(Reg8 value) { CPU::out8(IO::EEARL,value); }
    Reg8 eearh() { return CPU::in8(IO::EEARH); }
    void eearh(Reg8 value) { CPU::out8(IO::EEARH,value); }
    Reg16 eearhl() {
        Reg16 value = eearl();
	value |= ((Reg16)eearh()) << 8;
	return value;
    }
    void eearhl(Reg16 value) {
        eearh((Reg8)(value >> 8));
	eearl((Reg8)value);
    }
};

__END_SYS

#endif
