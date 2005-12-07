// EPOS-- Display Mediator Common Package
//

#ifndef __display_h
#define __display_h

#include <system/config.h>
#include <uart.h>

__BEGIN_SYS

class Display_Common
{
protected:
    Display_Common() {}
};

class Serial_Display: protected Display_Common
{
public:
    Serial_Display() {}

    void clear() {}

    void putc(char c){ _uart.txd(c); }

    void puts(const char * s) {
	while(*s != '\0')
	    putc(*s++);
    }

    void geometry(int * lines, int * columns) {
	*lines = -1;
	*columns = -1;
    }

    void position(int * line, int * column) { *line = *column = -1; }
    void position(int line, int column) {}

    static int init(System_Info * si);

private:
    UART _uart;
};

template <bool serial = true>
class Select_Display: public Serial_Display {};

__END_SYS

#include __HEADER_MACH(display)

#endif
