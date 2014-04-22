//ARM PrimeCell level 2 cache controller (PL310) Hardware Mediator

#ifndef __panda_cc_h
#define __panda_cc_h

#include <cpu.h>

__BEGIN_SYS

//PandaBoard has an L2-cache of 1MB
//physically addressed and tagged
class ARM_PL310
{
private:
    //PandaBoard PL310 base address
    static const unsigned int BASE_ADDRESS = 0x48242000;
    static const unsigned int SIZE = 0x1000; //4KB
    
    //memory regions
    enum {
        CIDCT   = 0x000, //Cache ID and Cache Type registers
        CTL     = 0x100, //Control registers
        ICCR    = 0x200, //Interrupt/Counter Control Registers
        CMO     = 0x700, //Cache Maintenance Operations registers
        CL      = 0x900, //Cache Lockdown registers
        AF      = 0xC00, //Address Filtering registers
        DP      = 0xF00, //Debug and Prefetch
    };
    
    //registers memory mapping
    enum {
        R0_CID              = 0x000, //Cache ID register
        R0_CTYPE            = 0x004, //Cache Type register
        R1_CTL              = 0x100, //Control register
        R1_AUX_CTL          = 0x104, //Auxiliary Control register
        R1_TAG_RAM_CTL      = 0x108, //Tag Latency Control register
        R1_DATA_RAM_CTL     = 0x10C, //Data RAM Latency Control register
        R2_EV_CR_CTL        = 0x200, //Event Counter Control register
        R2_EV_CR1_CFG       = 0x204, //Event Counter Configuration register 1
        R2_EV_CR0_CFG       = 0x208, //Event Counter Configuration register 0
        R2_EV_CR1           = 0x20C, //Event Counter value register 1
        R2_EV_CR0           = 0x210, //Event Counter value register 0
        R2_INT_MASK         = 0x214, //Interrupt registers
        R2_INT_MASK_STATUS  = 0x218, //Interrupt registers
        R2_INT_RAW_STATUS   = 0x21C, //Interrupt registers
        R2_INT_CLEAR        = 0x220, //Interrupt registers
        R7_CACHE_SYNC       = 0x730, //Cache Maintenance Operations
        R7_INV_PA           = 0x770, //Cache Maintenance Operations
        R7_INV_WAY          = 0x77C, //Cache Maintenance Operations
        R7_CLEAN_PA         = 0x7B0, //Cache Maintenance Operations
        R7_CLEAN_INDEX      = 0x7B8, //Cache Maintenance Operations
        R7_CLEAN_WAY        = 0x7BC, //Cache Maintenance Operations
        R7_CLEAN_INV_PA     = 0x7F0, //Cache Maintenance Operations
        R7_CLEAN_INV_INDEX  = 0x7F8, //Cache Maintenance Operations
        R7_CLEAN_INV_WAY    = 0x7FC, //Cache Maintenance Operations
        R9_D_LOCKDOWN0      = 0x900, //Cache Lockdown registers
        R9_I_LOCKDOWN0      = 0x904, //Cache Lockdown registers
        R9_D_LOCKDOWN1      = 0x908, //Cache Lockdown registers
        R9_I_LOCKDOWN1      = 0x90C, //Cache Lockdown registers
        R9_D_LOCKDOWN2      = 0x910, //Cache Lockdown registers
        R9_I_LOCKDOWN2      = 0x914, //Cache Lockdown registers
        R9_D_LOCKDOWN3      = 0x918, //Cache Lockdown registers
        R9_I_LOCKDOWN3      = 0x91C, //Cache Lockdown registers
        R9_D_LOCKDOWN4      = 0x920, //Cache Lockdown registers
        R9_I_LOCKDOWN4      = 0x924, //Cache Lockdown registers
        R9_D_LOCKDOWN5      = 0x928, //Cache Lockdown registers
        R9_I_LOCKDOWN5      = 0x92C, //Cache Lockdown registers
        R9_D_LOCKDOWN6      = 0x930, //Cache Lockdown registers
        R9_I_LOCKDOWN6      = 0x934, //Cache Lockdown registers
        R9_D_LOCKDOWN7      = 0x938, //Cache Lockdown registers
        R9_I_LOCKDOWN7      = 0x93C, //Cache Lockdown registers
        R9_LOCK_LINE_EN     = 0x950, //Cache Lockdown registers
        R9_UNLOCK_WAY       = 0x954, //Cache Lockdown registers
        R12_ADDR_FIL_START  = 0xC00, //Address Filtering
        R12_ADDR_FIL_END    = 0xC04, //Address Filtering
        R15_DEBUG_CTRL      = 0xF40, //Debug register
        PREFETCH_OFFSET_REG = 0xF60, //Prefetch register
    };
    
private:
    unsigned int _base;
    unsigned int _size;
};

__END_SYS

#endif

