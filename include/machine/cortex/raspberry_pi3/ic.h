// EPOS Paspberry Pi3 (ARM Cortex-A53) IC Mediator Declarations

#ifndef __raspberry_pi3_ic_h
#define __raspberry_pi3_ic_h

#define __common_only__
#include <machine/ic.h>
#undef __common_only__
#include <machine/cortex/engines/cortex_a53/bcm_mailbox.h>
#include "memory_map.h"

__BEGIN_SYS

class IC_Engine: public IC_Common
{
public:
    // Interrupts
    static const unsigned int INTS = Traits<IC>::INTS;
    static const unsigned int EXC_INT = 0; // Not mapped by IC. Exceptions are hard configured by SETUP.
    static const unsigned int SOFT_INT = 0;
    static const unsigned int INTS = 3 * 32 + 16 + 1;
    static const unsigned int LAST_INT = INTS;
    static const unsigned int HARD_INT = 0;
    static const unsigned int SOFT_INT = 0;
    enum {
        INT_TIMER       = SYSTEM_TIMER_MATCH1,
        INT_USER_TIMER0 = SYSTEM_TIMER_MATCH3,
        INT_USER_TIMER1 = 0,
        INT_USER_TIMER2 = 0,
        INT_USER_TIMER3 = 0,
        INT_USB0        = USB_CONTROLLER,
        INT_GPIOA       = GPIO_INT0,
        INT_GPIOB       = GPIO_INT1,
        INT_GPIOC       = GPIO_INT2,
        INT_GPIOD       = GPIO_INT3,
        INT_NIC0_RX     = 0,
        INT_NIC0_TX     = 0,
        INT_NIC0_ERR    = 0,
        INT_NIC0_TIMER  = 0,
        // INT_FIRST_HARD  = HARD_INT,
        // INT_LAST_HARD   = IRQ_PARITY,
        INT_RESCHEDULER = CORE0_MAILBOX0_IRQ,
        INT_RESCHEDULER0 = CORE0_MAILBOX0_IRQ,
        INT_RESCHEDULER1 = CORE1_MAILBOX0_IRQ,
        INT_RESCHEDULER2 = CORE2_MAILBOX0_IRQ,
        INT_RESCHEDULER3 = CORE3_MAILBOX0_IRQ
    };

    enum {
        INT_TIMER       = BCM_IC::IRQ_PRIVATE_TIMER,
        INT_USER_TIMER0 = BCM_IC::IRQ_GLOBAL_TIMER,
        INT_USER_TIMER1 = 0,
        INT_USER_TIMER2 = 0,
        INT_USER_TIMER3 = 0,
        INT_GPIOA       = BCM_IC::IRQ_GPIO,
        INT_GPIOB       = BCM_IC::IRQ_GPIO,
        INT_GPIOC       = BCM_IC::IRQ_GPIO,
        INT_GPIOD       = BCM_IC::IRQ_GPIO,
        INT_NIC0_RX     = BCM_IC::IRQ_ETHERNET0,
        INT_NIC0_TX     = BCM_IC::IRQ_ETHERNET0,
        INT_NIC0_ERR    = BCM_IC::IRQ_ETHERNET0,
        INT_NIC0_TIMER  = 0,
        INT_USB0        = BCM_IC::IRQ_USB0,
        INT_FIRST_HARD  = BCM_IC::HARD_INT,
        INT_LAST_HARD   = BCM_IC::IRQ_PARITY,
        INT_RESCHEDULER = BCM_IC::IRQ_SOFTWARE0,
        LAST_INT        = INT_RESCHEDULER
    };

public:
    static void enable() { gic_distributor()->enable(); }
    static void enable(const Interrupt_Id & id)  { gic_distributor()->enable(id); }
    static void disable() { gic_distributor()->disable(); }
    static void disable(const Interrupt_Id & id) { gic_distributor()->disable(); }

    static Interrupt_Id int_id() { return gic_cpu()->int_id(); }
    static Interrupt_Id irq2int(const Interrupt_Id & id) { return gic_distributor()->irq2int(id); }
    static Interrupt_Id int2irq(const Interrupt_Id & irq) { return gic_distributor()->int2irq(irq); }

    static void ipi(unsigned int cpu, const Interrupt_Id & id) { gic_distributor()->send_sgi(cpu, id); }

    static void init() {
        gic_distributor()->init();
        gic_cpu()->init();
    };

private:
    static BCM_IC_CPU * gic_cpu() { return reinterpret_cast<BCM_IC_CPU *>(Memory_Map::BCM_IC_CPU_BASE); }
    static BCM_IC_Distributor * gic_distributor() { return reinterpret_cast<BCM_IC_Distributor *>(Memory_Map::BCM_IC_DIST_BASE); }
};

__END_SYS

#endif
