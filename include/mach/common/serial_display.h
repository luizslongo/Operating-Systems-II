// EPOS-- Serial Display Declarations

#ifndef __serial_display_h
#define __serial_display_h

//#include <display.h>
#include <uart.h>

__BEGIN_SYS

class Serial_Display: public Display_Common
{
private:

    enum {
	ESC  = 0x1b,
	CR   = 0x0d,
	LF   = 0x0a,
	TAB  = 0x09,
    };

    static const int COLUMNS = 80;
    static const int LINES = 24;
    static const int TAB_SIZE = 5;

public:

    Serial_Display() : _line(0), _col(0) {}
    ~Serial_Display() {}

    void clear(){
	escape();
	_uart.put('2');
	_uart.put('J');
    };

    void putc(char c){

	switch(c) {
	case '\n':
	    scroll();
	    _line++;
	    break;
	case '\t':
	    _uart.put(TAB);
	    _col+=tab_size();
	    break;
	default:
	    _col++;
	    _uart.put(c);
	    if (_col >= columns()) scroll();
	}
    };

    void puts(const char * s) {
	while(*s != '\0')
	    putc(*s++);
    }

    void puti(unsigned long v) {
	
	unsigned long major = 1000000000;
	unsigned char digit;

	while(major >= 10) {
	    digit = '0';
	    while(v >= major) {
		digit++;
		v -= major;
	    }
	    _uart.put(digit);
	    major /= 10;
	}
	_uart.put('0' + v);

    }



    unsigned int lines() { return LINES; }
    unsigned int columns() { return COLUMNS; } ;

    void position(int * line, int * column) {
	*column = _col;
	*line = _line;
    }
    void position(int line, int column) {
	_line = line;
	_col = column;
	escape();
	_puti(_line);
	_uart.put(';');
	_puti(_col);
	_uart.put('H');	
    }


private:

    unsigned int tab_size() { return TAB_SIZE; };

    void escape(){
	_uart.put(ESC);
	_uart.put('[');
    }

    void _puti(unsigned char value){

	unsigned char digit = '0';
	while( value >= 100 ) {
	    digit++;     
	    value -= 100;
	}
	_uart.put(digit);
    
	digit = '0';
	while( value >= 10 ) {
	    digit++; 
	    value -= 10;
	}
	_uart.put(digit);

	_uart.put( '0' + value  );
    
    }

    void scroll() {
	_uart.put(CR);
	_uart.put(LF);
	_col = 0;
    }



private:
    unsigned int _line;
    unsigned int _col;
    UART _uart;
    
};

__END_SYS

#endif
