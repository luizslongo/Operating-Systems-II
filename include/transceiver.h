// EPOS-- Transceiver Mediator Common Package

#ifndef __transceiver_h
#define __transceiver_h

#include <system/config.h>

__BEGIN_SYS

class Transceiver_Common {

public:
	typedef unsigned int microseconds_t;

	typedef enum {
		SUCCESS,
		BUSY,
		FAILED,
		FAILED_OPERATING_MODE,
		TIME_OUT,
	} result_t;

	typedef enum {
		FRAME_RECEIVED,
		FRAME_TRANSMITED,
	} event_t;

	typedef void (event_handler)(event_t);

protected:
	Transceiver_Common(){}

};

__END_SYS


#ifdef __TRANSCEIVER_H
#include __TRANSCEIVER_H
#endif

#endif /* TRANSCEIVER_H_ */
