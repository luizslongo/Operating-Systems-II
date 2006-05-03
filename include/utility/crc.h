#ifndef __crc_h
#define __crc_h

__BEGIN_SYS

class CRC {

public:

    static unsigned short crc16(char *ptr, int count){
	unsigned short crc;
	char i;

	crc = 0;
	while (--count >= 0){
	    crc = crc ^ (int) *ptr++ << 8;
	    i = 8;
	    do{
		if (crc & 0x8000)
		    crc = crc << 1 ^ 0x1021;
		else
		    crc = crc << 1;
	    } while(--i);
	}
	return (crc);
    }

};


__END_SYS


#endif
