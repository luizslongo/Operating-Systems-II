// EPOS PC Mediator Initialization

#include <machine.h>

__BEGIN_SYS

void Machine::init()
{
    db<Init, Machine>(TRC) << "Machine::init()" << endl;

    if(Traits<IC>::enabled)
        IC::init();

    if(Traits<Timer>::enabled)
        Timer::init();

    if(Traits<PCI>::enabled)
        PCI::init();

#if defined(__SCRATCHPAD_H) && !defined(__common_only__)
    if(Traits<Scratchpad>::enabled)
        Scratchpad::init();
#endif

#if defined(__KEYBOARD_H) && !defined(__common_only__)
    if(Traits<Keyboard>::enabled)
        Keyboard::init();
#endif

#if defined(__FPGA_H) && !defined(__common_only__)
    if(Traits<FPGA>::enabled)
        FPGA::init();
#endif

#if defined(__NIC_H) && !defined(__common_only__)
    if(Traits<Ethernet>::enabled)
        Initializer<Ethernet>::init();
#endif
}

__END_SYS
