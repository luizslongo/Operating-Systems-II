//ARM Snoop Control Unit Hardware Mediator

#ifndef __panda_scu_h
#define __panda_scu_h

#include <cpu.h>

__BEGIN_SYS

//SCU connects one to four processors to the memory system through the AXI interfaces
//The main functions are
// - keep data coherency among processors
// - initiate L2 AXI memory accesses
// - arbitrate between processors requesting L2 accesses
// - manage ACP accesses

class PandaBoard_SCU
{
private:
    //PandaBoard SCU base address
    static const unsigned int BASE_ADDRESS = 0x48240000;
    static const unsigned int SIZE = 0x80; //128 bytes
    
    //SCU registers mapping
    enum {
        CONTROL         = 0x00, //SCU control register
        CONFIG          = 0x04, //SCU configuration register - read only
        POWER           = 0x08, //SCU CPU power status register
        INVALIDATE      = 0x0C, //SCU invalidate all registers in secure state - reset value = 0x00000000 - write only, reads return 0
        FILTER_START    = 0x40, //Filtering start address register
        FILTER_END      = 0x44, //Filtering end address register
        SAC             = 0x50, //SCU Access Control register - reset value = 0x0000000F
        SNSAC           = 0x54, //SCU Non-Secure Access Control register - reset value = 0x00000000
    };
    
    //SCU control register bits
    enum {
        SCU_ENABLE      = 0x01,
        FILTERING       = (1 << 1),
        RAM_PARITY      = (1 << 2),
        LINEFILL        = (1 << 3),
        PORT0           = (1 << 4),
        SCU_STANDBY     = (1 << 5),
        IC_STANDBY      = (1 << 6)        
    };

public:
    
    //enables SCU
    static void enable() {
        CPU::out32(BASE_ADDRESS + CONTROL, SCU_ENABLE);
    }
    
    //disables SCU
    static void disable() {
        CPU::out32(BASE_ADDRESS + CONTROL, ~SCU_ENABLE);
    }
    
    //returns true if SCU is enabled
    static bool enabled() {
        return (CPU::in32(BASE_ADDRESS + CONTROL) & SCU_ENABLE);
    }
    
    //returns the configuration register - read only
    static unsigned int config() {
        return CPU::in32(BASE_ADDRESS + CONFIG);
    }
    
    /* invalidate all lines in the selected ways
     * 0-3   ways that must be invalidate for CPU0
     * 4-7   ways that must be invalidate for CPU1
     * 8-11  ways that must be invalidate for CPU0
     * 12-15 ways that must be invalidate for CPU0 */
    static void invalidate(unsigned short ways) {
        CPU::out32(BASE_ADDRESS + INVALIDATE, ways);
    }
    
    static void init() {
        db<Init, PandaBoard>(TRC) << "SCU init() CONTROL = " << (void *) CPU::in32(BASE_ADDRESS + CONTROL) << "\n";
        enable();        
        db<Init, PandaBoard>(TRC) << "SCU init() END CONTROL = " << (void *) CPU::in32(BASE_ADDRESS + CONTROL) << "\n";
    }

};

__END_SYS

#endif

