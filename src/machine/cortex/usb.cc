// EPOS Cortex USB Mediator Initialization

#include <machine/ic.h>
#include <machine/usb.h>

#ifdef __USB_H

__BEGIN_SYS

void USB::int_handler(const IC::Interrupt_Id & i) {
    Engine usb; usb.handle_int(i);
}

void USB::eoi(const IC::Interrupt_Id & int_id) {
    Engine usb; usb.eoi();
}

__END_SYS

#endif
