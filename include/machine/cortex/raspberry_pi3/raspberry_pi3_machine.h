// EPOS Raspberry Pi3 (Cortex-A53) Mediator Declarations

#ifndef __raspberry_pi3_machine_h
#define __raspberry_pi3_machine_h

#include <architecture/cpu.h>
#include <architecture/tsc.h>
#include <machine/rtc.h>
#include <system.h>

__BEGIN_SYS

class Raspberry_Pi3
{
protected:
    typedef CPU::Reg32 Reg32;
    typedef CPU::Log_Addr Log_Addr;

public:

    // Base addresses for memory-mapped I/O devices
    enum {
        UART1_BASE                  = 0x3f215000, // PrimeCell PL011 UART
        GPIOA_BASE                  = 0x3f200000, // PrimeCell PL061 GPIO
        DMA_BASE                    = 0x3fe05000, // PrimeCell PL080 DMA Controller
        MBOX_COM                    = 0x3ef00000, // basic memory for device-os communication
        MBOX_COM_CPU_OFFSET         = 0x00040000, // memory offset per cpu
        MBOX_SIZE                   = 0x00000100, // mbox size (rounded up)

        PPS_BASE                    = 0x3f000000, // A9 Private Peripheral Space
        IC_BASE                     = 0x3f00b200, // BCM2870 IC
        ARM_TIMER_BASE              = 0x3f00b400, // ARM Timer (frequency relative to processor frequency)
        SYSTEM_TIMER_BASE           = 0x3f003000, // System Timer (free running)
        BMC_CONTROL                 = 0x40000000
    };

    // BMC_CONTROL Registers
    enum {
        GPU_INT_ROUTING             = 0x00C,
        PMU_INT_ROUTING_SET         = 0x010,
        PMU_INT_ROUTING_CLR         = 0x014,
        LOCAL_INT_ROUTING           = 0x024, // Timer Interrupt
        LOCAL_TIMER_CS              = 0x034, // Control / Satus
        LOCAL_TIMER_WFLAGS          = 0x038, //

        CORE0_TIMER_INT_CTRL        = 0x040,
        CORE1_TIMER_INT_CTRL        = 0x044,
        CORE2_TIMER_INT_CTRL        = 0x048,
        CORE3_TIMER_INT_CTRL        = 0x04c,

        CORE0_MBOX_INT_CTRL         = 0x050,
        CORE1_MBOX_INT_CTRL         = 0x054,
        CORE2_MBOX_INT_CTRL         = 0x058,
        CORE3_MBOX_INT_CTRL         = 0x05C,

        CORE0_IRQ_SRC               = 0x060,
        CORE1_IRQ_SRC               = 0x064,
        CORE2_IRQ_SRC               = 0x068,
        CORE3_IRQ_SRC               = 0x06c,

        MBOX_WS_BASE                = 0x080, // Each CPU has 4 Mailboxes WRITE-SET   registers of 4 Bytes
        MBOX_WC_BASE                = 0x0c0  // Each CPU has 4 Mailboxes WRITE-CLEAR registers of 4 Bytes
    };

    // Useful bits in the CONTROL_REG0 register
    enum {                                      // Description              Type    Value after reset
        RXRES                       = 1 << 0,   // Reset Rx data path       r/w     0
        TXRES                       = 1 << 1,   // Reset Tx data path       r/w     0
        RXEN                        = 1 << 2,   // Receive enable           r/w     0
        TXEN                        = 1 << 4    // Transmit enable          r/w     0
    };

    // Useful bits in the MODE_REG0 register
    enum {                                      // Description              Type    Value after reset
        CHRL8                       = 0 << 1,   // Character Length 8 bits  r/w     0
        CHRL7                       = 2 << 1,   // Character Length 7 bits  r/w     0
        CHRL6                       = 3 << 1,   // Character Length 6 bits  r/w     0
        PAREVEN                     = 0 << 3,   // Even parity              r/w     0
        PARODD                      = 1 << 3,   // Odd parity               r/w     0
        PARNONE                     = 4 << 3,   // No parity                r/w     0
        NBSTOP2                     = 2 << 6,   // 2 stop bits              r/w     0
        NBSTOP1                     = 0 << 6,   // 1 stop bit               r/w     0
        CHMODENORM                  = 0 << 8,   // Normal mode              r/w     0
        CHMODELB                    = 2 << 8    // Loopback mode            r/w     0
    };

    //COREX_IRQ_SRC usefull bits
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

    // LOCAL_TIMER_CS usefull bits
    enum {
        LT_INT_ENABLE               = 1 << 29,
        LT_ENABLE                   = 1 << 28,
        LOAD_VALUE                  = 1 << 0
    };

    // LOCAL_TIMER_WFLAGS usefull bits
    enum {
        LT_INT_CLEAR               = 1 << 31,
        LT_RELOAD                  = 1 << 30
    };

    // COREX_MBOX_INT_CTRL usefull bits
    enum {
        MBOX_IRQ_0                  = 1 << 0,
        MBOX_IRQ_1                  = 1 << 1,
        MBOX_IRQ_2                  = 1 << 2,
        MBOX_IRQ_3                  = 1 << 3
    };

    // COREX_TIMER_INT_CTRL usefull bits
    enum {
        CNT_PSIRQ                   = 1 << 0,
        CNT_PNSIRQ                  = 1 << 1,
        CNT_HPIRQ                   = 1 << 2,
        CNT_VIRQ                    = 1 << 3
    };

    // IC Registers (as offsets to IC_BASE)
    enum {                                   // Type    Value after reset
        IRQ_BASIC_PENDING           = 0x000, // r       0x000
        IRQ_PENDING_1               = 0x004, // r       0x000
        IRQ_PENDING_2               = 0x008, // r/w     0x000
        FIQ_CONTROL                 = 0x00C, // r/w     0x000
        ENABLE_IRQS_1               = 0x010, // r/w     0x000
        ENABLE_IRQS_2               = 0x014, // r/w     0x000
        ENABLE_BASIC_IRQS           = 0x018, // r/w     0x000
        DISABLE_IRQS_1              = 0x01C, // r/w     0x000
        DISABLE_IRQS_2              = 0x020, // r/w     0x000
        DISABLE_BASIC_IRQS          = 0x024  // r/w     0x000
    };

    //usefull bits in IRQ_BASIC
    enum {
        ARM_TIMER                   = 1 << 0,
        ARM_MAILBOX                 = 1 << 1
    };

    //usefull bits in IRQ_1
    enum {
        SYSTEM_C1                   = 1 << 1,
        SYSTEM_C3                   = 1 << 3
    };

    // Useful bits in the INTRPT_EN_REG0, and INTRPT_DIS_REG0 registers
    // enum {                                      // Description              Type    Value after reset
    //     INTRPT_RTRIG                = 1 << 0,   // Receiver FIFO empty      wo      0
    //     INTRPT_TTRIG                = 1 << 10   // Transmitter FIFO trigger wo      0
    // };

    // Useful bits in the CHANNEL_STS_REG0 register
    // enum {                                      // Description              Type    Value after reset
    //     STS_RTRIG                   = 1 << 0,   // Receiver FIFO trigger    ro      0
    //     STS_TFUL                    = 1 << 4    // Transmitter FIFO full    ro      0
    // };

    // SLCR Registers offsets
    // enum {                                      // Description
    //     SLCR_LOCK                   = 0x004,    // Lock the SLCR
    //     SLCR_UNLOCK                 = 0x008,    // Unlock the SLCR
    //     UART_CLK_CTRL               = 0x154,    // UART Ref Clock Control
    //     FPGA0_CLK_CTRL              = 0x170,    // PL Clock 0 Output control
    //     PSS_RST_CTRL                = 0x200,    // PS Software Reset Control
    //     FPGA_RST_CTRL               = 0x240     // FPGA Software Reset Control
    // };

    // Useful bits in SLCR_LOCK
    // enum {                                      // Description                  Type    Value after reset
    //     LOCK_KEY                    = 0x767B    // Lock key                     wo      0
    // };

    // Useful bits in SLCR_UNLOCK
    // enum {                                      // Description                  Type    Value after reset
    //     UNLOCK_KEY                  = 0xDF0D    // Unlock key                   wo      0
    // };

    // Useful bits in FPGAN_CLK_CTRL
    // enum {                                      // Description                  Type    Value after reset
    //     DIVISOR0                    = 1 << 8,   // First cascade divider        r/w     0x18
    //     DIVISOR1                    = 1 << 20   // Second cascade divider       r/w     0x1
    // };

    // SYSTEM TIMER
    enum {                                      // Description
        STCS                        = 0x00,     // Control/Status
        STCLO                       = 0x04,     // Low COUNTER
        STCHI                       = 0x08,     // High Counter
        STC0                        = 0x0C,     // Compare 0 - Used by GPU
        STC1                        = 0x10,     // Compare 1 - Value used to generate interrupt 1
        STC2                        = 0x14,     // Compare 2 - Used by GPU
        STC3                        = 0x18      // Compare 3 - Value used to generate interrupt 3
        // Interrupts mapped to "Enable IRQ 1" - c1 and c3 == irq1 and irq3
    };

    // Useful bits in CS write one to clear interrupt
    enum {
        STCSM0                      = 1 << 0, // Clear Interrup Compare 0
        STCSM1                      = 1 << 1, // Clear Interrup Compare 1
        STCSM2                      = 1 << 2, // Clear Interrup Compare 2
        STCSM3                      = 1 << 3, // Clear Interrup Compare 3
        TIMER_ENABLE                = STCSM1
    };

    // ARM TIMER
    enum {                                      // Description
        ATLR                        = 0x00,     // Load
        ATCTR                       = 0x04,     // Counter
        ATCLR                       = 0x08,     // Control
        ATISR                       = 0x0C,     // Interrupt Status
        ATRLD                       = 0x18,     // Reload (Update reload without reset counting)
        ATDIV                       = 0x1C
    };

    // Useful bits in ATCLR
    enum {                                      // Description                  Type    Value after reset
        RESET_VALUE                 = 0x003E0000,
        ARM_TIMER_ENABLE            = 1 << 7,   // Enable                       r/w     0
        IRQ_EN                      = 1 << 5,    // Enable interrupt             r/w     0
        ATDIV_VALUE                 = 0xF9
    };

    // Useful bits in PTISR
    enum {                                      // Description                  Type    Value after reset
        INT_CLR                     = 0    // Interrupt clear bit          r/w     0
    };

protected:
    Raspberry_Pi3() {}

    static void reboot() {}

    static unsigned int cpu_id() {
        int id;
        ASM("mrc p15, 0, %0, c0, c0, 5" : "=r"(id) : : );
        return id & 0x3;
    }

    static unsigned int n_cpus() {
        int n;
        ASM("mrc p15, 4, %0, c15, c0, 0 \t\n\
             ldr %0, [%0, #0x004]" : "=r"(n) : : );
        return (n & 0x3) + 1;
    }

    static void smp_init(unsigned int n_cpus);
        // Realview_PBX init changed in order to only wakeup n-1 cores

    static void enable_uart(unsigned int unit) {}

    // Power Management
    static void power_uart(unsigned int unit, const Power_Mode & mode) {
        assert(unit < UARTS);
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL:
            break;
        case LIGHT:
            break;
        case SLEEP:
            break;
        case OFF:
            break;
        }
    }

    static void power_user_timer(unsigned int unit, const Power_Mode & mode) {
        assert(unit < UARTS);
        switch(mode) {
        case ENROLL:
            break;
        case DISMISS:
            break;
        case SAME:
            break;
        case FULL:
            break;
        case LIGHT:
        case SLEEP:
        case OFF:
            break;
        }
    }


public:
    static volatile Reg32 & scu(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SCU_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & global_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(GLOBAL_TIMER_BASE)[o / sizeof(Reg32)]; }
    static volatile Reg32 & private_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(SYSTEM_TIMER_BASE)[o / sizeof(Log_Addr)]; }
    static volatile Reg32 & arm_timer(unsigned int o) { return reinterpret_cast<volatile Reg32 *>(ARM_TIMER_BASE)[o / sizeof(Log_Addr)]; }

    static void pre_init();
    static void init();
};

typedef Raspberry_Pi3 Machine_Model;

__END_SYS

#endif
