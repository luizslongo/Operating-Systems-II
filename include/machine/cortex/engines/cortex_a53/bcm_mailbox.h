// EPOS ARM BCM2835 (Cortex-A53) IC Mediator Declarations

#ifndef __bcm2853_ic_h
#define __bcm2853_ic_h

#include <architecture/cpu.h>
#define __common_only__
#include <machine/ic.h>
#undef __common_only__

__BEGIN_SYS

class BMC_IC: public IC_Common
{
protected:
    typedef CPU::Reg32 Reg32;

public:
    // IRQs
    static const unsigned int IRQS = Traits<IC>::IRQS;
    typedef Interrupt_Id IRQ;
    enum {
        // Interrupts not listed here should not be enabled as they will interfere with the GPU operation

        // IRQ1
        SYSTEM_TIMER_MATCH1           = 1,
        SYSTEM_TIMER_MATCH3           = 3,
        USB_CONTROLLER                = 9,
        UART_AUX_INT                  = 29,

        // IRQ2
        I2C_SPI_SLV_INT               = 43,
        PWA0                          = 45,
        PWA1                          = 46,
        SMI                           = 48,
        GPIO_INT0                     = 49,
        GPIO_INT1                     = 50,
        GPIO_INT2                     = 51,
        GPIO_INT3                     = 52,
        I2C_INT                       = 53,
        SPI_INT                       = 54,
        PCM_INT                       = 55,
        UART_INT0                     = 57,

        // FIQ
        ARM_TIMER_IRQ                 = 64,
        ARM_MAILBOX_IRQ               = 65,
        ARM_DOORBELL_0_IRQ            = 66,
        ARM_DOORBELL_1_IRQ            = 67,
        GPU_0_HALTED_IRQ              = 68,
        GPU_1_HALTED_IRQ              = 69,
        ACCESS_ERROR_TYPE_1_IRQ       = 70,
        ACCESS_ERROR_TYPE_0_IRQ       = 71,

        // MAILBOXES
        CORE0_MAILBOX0_IRQ            = 96,
        CORE0_MAILBOX1_IRQ            = 97,
        CORE0_MAILBOX2_IRQ            = 98,
        CORE0_MAILBOX3_IRQ            = 99,
        CORE1_MAILBOX0_IRQ            = 100,
        CORE1_MAILBOX1_IRQ            = 101,
        CORE1_MAILBOX2_IRQ            = 102,
        CORE1_MAILBOX3_IRQ            = 103,
        CORE2_MAILBOX0_IRQ            = 104,
        CORE2_MAILBOX1_IRQ            = 105,
        CORE2_MAILBOX2_IRQ            = 106,
        CORE2_MAILBOX3_IRQ            = 107,
        CORE3_MAILBOX0_IRQ            = 108,
        CORE3_MAILBOX1_IRQ            = 109,
        CORE3_MAILBOX2_IRQ            = 110,
        CORE3_MAILBOX3_IRQ            = 111
    };
};

class BMC_Mailbox: public BMC_IC
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::SPIx_BASE) BCM_IC".

private:
    typedef CPU::Reg32 Reg32;

public:
    // Registers offsets from BASE (i.e. this)
    enum {
        GPU_INT_ROUTING             = 0x00c,
        PMU_INT_ROUTING_SET         = 0x010,
        PMU_INT_ROUTING_CLR         = 0x014,
        LOCAL_INT_ROUTING           = 0x024, // Timer interrupt
        LOCAL_TIMER_CS              = 0x034, // Control/Status
        LOCAL_TIMER_WFLAGS          = 0x038, //

        CORE0_TIMER_INT_CTRL        = 0x040,
        CORE1_TIMER_INT_CTRL        = 0x044,
        CORE2_TIMER_INT_CTRL        = 0x048,
        CORE3_TIMER_INT_CTRL        = 0x04c,

        CORE0_MBOX_INT_CTRL         = 0x050,
        CORE1_MBOX_INT_CTRL         = 0x054,
        CORE2_MBOX_INT_CTRL         = 0x058,
        CORE3_MBOX_INT_CTRL         = 0x05c,

        CORE0_IRQ_SRC               = 0x060,
        CORE1_IRQ_SRC               = 0x064,
        CORE2_IRQ_SRC               = 0x068,
        CORE3_IRQ_SRC               = 0x06c,

        MBOX_WS_BASE                = 0x080, // Each CPU has 4 Mailboxes WRITE-SET   registers of 4 Bytes
        MBOX_WC_BASE                = 0x0c0  // Each CPU has 4 Mailboxes WRITE-CLEAR registers of 4 Bytes
    };

    // COREx_IRQ_SRC useful bits
    enum {
        SRC_CNTPS                   = 1 << 0,
        SRC_CNTPNS                  = 1 << 1,
        SRC_CNTHP                   = 1 << 2,
        SRC_CNTV                    = 1 << 3,
        SRC_MBOX_1                  = 1 << 4,
        SRC_MBOX_2                  = 1 << 5,
        SRC_MBOX_3                  = 1 << 6,
        SRC_MBOX_4                  = 1 << 7
    };

public:
    BMC_Mailbox() {}

    static int irq2int(int i) { return i; }
    static int int2irq(int i) { return i; }

    static void enable() {}

    static void enable(int i) {
        assert(i <= INTS);
        switch(i/32){
            case 0:
            case 1:
            case 2:
                ic_irq_enable((i/32)*4) |= 1 << (i%32);
                break;
            default:
                ic_mailbox_int((i%32)) |= 1 << i%4;
                break;
        }
    }

    static void disable() {}

    static void disable(int i) {
        assert(i < INTS);
        switch(i/32){
            case 0:
            case 1:
            case 2:
                ic_irq_disable((i/32)*4) |= 1 << (i%32);
                break;
            default:
                ic_mailbox_int((i%32)) &= ~(1 << i%4);
                break;
        }
    }

    static void mailbox_eoi(const Interrupt_Id & int_id) {
        unsigned int cpu;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(cpu) : : );
        cpu &= 0x3;
        ic_mailbox_wc(cpu*16) = 1 << 31;//ic_mailbox_ws(cpu*16 + (int_id%4)*4); // ACK
    }

    static Interrupt_Id int_id() {
        unsigned int cpu;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(cpu) : : );
        cpu &= 0x3;
        Reg32 src = mailbox(CORE0_IRQ_SRC + 4 * cpu);
        if(src & 0x10) // mailbox 0 //ic_mailbox_wc(cpu*16) = ic_mailbox_ws(cpu*16); // ACK
            return CORE0_MAILBOX0_IRQ + 4 * cpu;
        else if (src & 0x20) // mailbox 1
            return CORE0_MAILBOX1_IRQ + 4 * cpu;
        else if (src & 0x40) // mailbox 2
            return CORE0_MAILBOX2_IRQ + 4 * cpu;
        else if (src & 0x80) // mailbox 3
            return CORE0_MAILBOX3_IRQ + 4 * cpu;

        Reg32 basic_irqs_pending = ic_irq_pending(0);
        if (basic_irqs_pending) {
            if (basic_irqs_pending & 1) {
                //(volatile unsigned int *)(ARM_TIMER_BASE+ATISR) = 1; // ACK EOI
                return ARM_TIMER_IRQ;
            }
            // return 63 + basic_irqs_pending; // TODO
        }

        Reg32 irqs1_pending = ic_irq_pending(1*4);
        if (irqs1_pending) {
            if (irqs1_pending & 2) {
                //(volatile unsigned int *)(SYSTEM_TIMER_BASE) |= 1 << 1; //ACK EOI
                return SYSTEM_TIMER_MATCH1; // handler needs to reset counter compare register
            }
            if (irqs1_pending & 4) {
                //(volatile unsigned int *)(SYSTEM_TIMER_BASE) |= 1 << 3; //ACK EOI
                return SYSTEM_TIMER_MATCH3; // handler needs to reset counter compare register
            }
            return SYSTEM_TIMER_MATCH1; //TODO
        }
        Reg32 irqs2_pending = ic_irq_pending(2*4);
        return irqs2_pending;
    }

    static void ipi(unsigned int cpu, const Interrupt_Id & id) {
        ic_mailbox_ws(cpu*16) = 1 << 31;
    }

    static void init(void) {
        // volatile unsigned int *periph = (volatile unsigned int *)0x40000000;
        ic_mailbox_int(0*4) = 0xF; //0x50/sizeof(int) = 5*16/4 = 5 * 4 = 20
        ic_mailbox_int(1*4) = 0xF; //0x54/sizeof(int) = 5*16/4 + 4/4 = 5 * 4 +1 = 21
        ic_mailbox_int(2*4) = 0xF; //0x58/sizeof(int) = 5*16/4 + 8/4 = 5 * 4 +2 = 22
        ic_mailbox_int(3*4) = 0xF; //0x5C/sizeof(int) = 5*16/4 + 12/4 = 5 * 4 +3 = 23
    }

protected:
    volatile Reg32 & mailbox(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(this)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_ws(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + MBOX_WS_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_wc(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + MBOX_WC_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_mailbox_int(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(BMC_CONTROL + CORE0_MBOX_INT_CTRL)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_enable(unsigned int o)  { return reinterpret_cast<volatile Reg32 *>(IC_BASE+ENABLE_IRQS_1)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_disable(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IC_BASE+DISABLE_IRQS_1)[o / sizeof(Reg32)]; }
    static volatile Reg32 & ic_irq_pending(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(IC_BASE)[o / sizeof(Reg32)]; }

protected:
    static const unsigned int INT_ID_MASK = 0x3ff;
};


class BMC_ARM_IC: public BCM_IC
{
    // This is a hardware object.
    // Use with something like "new (Memory_Map::SPIx_BASE) BCM_IC".

private:
    typedef CPU::Reg32 Reg32;

public:
    // IC Registers (as offsets to IC_BASE)
    enum {                                   // Type    Value after reset
        IRQ_BASIC_PENDING           = 0x000, // r       0x000
        IRQ_PENDING_1               = 0x004, // r       0x000
        IRQ_PENDING_2               = 0x008, // r/w     0x000
        FIQ_CONTROL                 = 0x00c, // r/w     0x000
        ENABLE_IRQS_1               = 0x010, // r/w     0x000
        ENABLE_IRQS_2               = 0x014, // r/w     0x000
        ENABLE_BASIC_IRQS           = 0x018, // r/w     0x000
        DISABLE_IRQS_1              = 0x01c, // r/w     0x000
        DISABLE_IRQS_2              = 0x020, // r/w     0x000
        DISABLE_BASIC_IRQS          = 0x024  // r/w     0x000
    };

    enum {
        ATISR                       = 0x0C,     // Interrupt Status ARM Timer
        STCLO                       = 0x04,     // Low COUNTER
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC3                        = 0X18,     // Compare 3 - Value used to generate interrupt 3
        STCS                        = 0x0       // Control/Status System Timer
    };

};
__END_SYS

#endif
