// EPOS LM3S811 (ARM Cortex) MCU Initialization

#include <system/config.h>
#include <machine.h>

#ifdef __mmod_lm3s811__

__BEGIN_SYS

void Machine_Model::init()
{
    db<Init, Machine>(TRC) << "LM3S811::init()" << endl;

    // Initialize the clock
    CPU::Reg32 rcc = scr(RCC);

    // bypass PLL and system clock divider while initializing
    rcc |= RCC_BYPASS;
    rcc &= ~RCC_USESYSDIV;
    scr(RCC) = rcc;

    // select the crystal value and oscillator source
    rcc &= ~RCC_XTAL_8192;
    rcc |= RCC_XTAL_6000;
    rcc |= RCC_MOSC;

    // activate PLL by clearing PWRDN and OEN
    rcc &= ~RCC_PWRDN;
    rcc &= ~RCC_OEN;

    // set the desired system divider and the USESYSDIV bit
    rcc |= RCC_SYSDIV_4;
    rcc |= RCC_USESYSDIV;
    scr(RCC) = rcc;

    // wait for the PLL to lock by polling PLLLRIS
    while(!(scr(RIS) & RIS_PLLLRIS));

    // enable use of PLL by clearing BYPASS
    rcc &= ~RCC_BYPASS;
    scr(RCC) = rcc;

    db<Init, Machine>(TRC) << "Machine_Model::init:CCR = " << scs(CCR) << endl;
    scs(CCR) |= BASETHR; // BUG: on LM3S811 this register is not updated, but it doesn't seem to cause any errors
    db<Init, Machine>(TRC) << "Machine_Model::init:CCR = " << scs(CCR) << endl;
}

__END_SYS

#endif
