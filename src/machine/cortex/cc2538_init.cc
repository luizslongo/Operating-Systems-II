// EPOS Cortex TI CC2538 IEEE 802.15.4 NIC Mediator Initialization

#include <system.h>
#ifndef __mmod_zynq__

#include <machine/cortex/machine.h>
#include <machine/cortex/cc2538.h>

__BEGIN_SYS

CC2538::CC2538(unsigned int unit): _unit(unit)
{
    db<CC2538>(TRC) << "CC2538(unit=" << unit << ")" << endl;

    // Set Address
    ffsm(SHORT_ADDR0) = _address[0];
    ffsm(SHORT_ADDR1) = _address[1];
    _address[0] = ffsm(SHORT_ADDR0);
    _address[1] = ffsm(SHORT_ADDR1);

    // Enable frame filtering
    xreg(FRMFILT0) |= FRAME_FILTER_EN;
    xreg(FRMFILT1) &= ~ACCEPT_FT2_ACK; // ACK frames are handled only when expected

    // Enable automatic source address matching
    xreg(SRCMATCH) |= SRC_MATCH_EN;

    // Enable auto-CRC
    xreg(FRMCTRL0) |= AUTO_CRC;

    channel(15);

    // Enable auto ACK
    xreg(FRMCTRL0) |= AUTO_ACK;

    // Reset statistics
    reset();

    xreg(FRMCTRL1) |= SET_RXENMASK_ON_TX; // Enter receive mode after TX

    // Enable useful device interrupts
    // WARNING: do not enable INT_TXDONE, because _send_and_wait handles it
    xreg(RFIRQM0) = INT_FIFOP;
    xreg(RFIRQM1) = 0;
    xreg(RFERRM) = 0;

    // Issue the listen command
    sfr(RFST) = ISTXON;
}


void CC2538::init(unsigned int unit)
{
    db<Init, CC2538>(TRC) << "Radio::init(unit=" << unit << ")" << endl;

    // Initialize the device
    CC2538 * dev = new (SYSTEM) CC2538(unit);

    // Register the device
    _devices[unit].device = dev;
    _devices[unit].interrupt = IC::irq2int(NVIC::IRQ_RFTXRX);

    // Install interrupt handler
    IC::int_vector(_devices[unit].interrupt, &int_handler);

    // Enable interrupts for device
    IC::enable(NVIC::IRQ_RFTXRX);
}

void CC2538::Timer::init()
{
    mactimer(MTCTRL) |= MTCTRL_RUN; // Stop counting
    mactimer(MTIRQM) = 0; // Mask interrupts
    mactimer(MTIRQF) = 0; // Clear interrupts
    mactimer(MTCTRL) &= ~MTCTRL_SYNC; // We can't use the sync feature because we want to change the count and overflow values when the timer is stopped
    mactimer(MTCTRL) |= MTCTRL_LATCH_MODE; // count and overflow will be latched at once
    IC::int_vector(IC::irq2int(NVIC::IRQ_MACTIMER), &int_handler);
    IC::enable(33);
    int_enable(INT_OVERFLOW_PER);
}

__END_SYS

#endif
