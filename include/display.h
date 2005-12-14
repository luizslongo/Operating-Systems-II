// EPOS-- Display Mediator Common Package

#ifndef __display_h
#define __display_h

#include <uart.h>

__BEGIN_SYS

class Display_Common
{
protected:
    Display_Common() {}
};

class Serial_Display: public Display_Common
{
private:
    static const int LINES = Traits<Serial_Display>::LINES;
    static const int COLUMNS = Traits<Serial_Display>::COLUMNS;
    static const int TAB_SIZE = Traits<Serial_Display>::TAB_SIZE;

    // Special characters
    enum {
	ESC  = 0x1b,
	CR   = 0x0d,
	LF   = 0x0a,
	TAB  = 0x09,
    };

public:
    Serial_Display() : _line(0), _column(0) {}

    void clear(){
	escape();
	_uart.put('2');
	_uart.put('J');
    };

    void putc(char c) {
	switch(c) {
	case '\n':
	    scroll();
	    _line++;
	    break;
	case '\t':
	    _uart.put(TAB);
	    _column += TAB_SIZE;
	    break;
	default:
	    _column++;
	    _uart.put(c);
	    if(_column >= COLUMNS) scroll();
	}
    };

    void puts(const char * s) {
	while(*s != '\0')
	    putc(*s++);
    }

    void geometry(int * lines, int * columns) {
	*lines = LINES;
	*columns = COLUMNS;
    }

    void position(int * line, int * column) {
	*line = _line;
	*column = _column;
    }
    void position(int line, int column) {
	_line = line;
	_column = column;
	escape();
	puti(_line);
	_uart.put(';');
	puti(_column);
	_uart.put('H');	
    }

    static int init(System_Info * si);

private:
    void escape() {
	_uart.put(ESC);
	_uart.put('[');
    }

    void puti(unsigned char value) {
	unsigned char digit = '0';
	while(value >= 100) {
	    digit++;     
	    value -= 100;
	}
	_uart.put(digit);
    
	digit = '0';
	while(value >= 10) {
	    digit++; 
	    value -= 10;
	}
	_uart.put(digit);

	_uart.put('0' + value);
    }

   void scroll() {
	_uart.put(CR);
	_uart.put(LF);
	_column = 0;
   }

private:
    UART _uart;
    int _line;
    int _column;
};

__END_SYS

#ifdef __DISPLAY_H
#include __DISPLAY_H
#endif

#endif
