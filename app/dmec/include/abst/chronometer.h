/*
 * chronometer.h
 *
 *  Created on: Feb 4, 2011
 *      Author: mateus
 */

#if 0

#ifndef CHRONOMETER_H_
#define CHRONOMETER_H_

#include <dmec_java/abst/chronometer_internals.h>

void setUpChronometer(Chronometer * _this);


void chrono_start(Chronometer * _this);


void chrono_pause(Chronometer * _this);


void chrono_clear(Chronometer * _this);


Second chrono_read_seconds(Chronometer * _this);


Milisecond chrono_read_milis(Chronometer * _this);


Microsecond chrono_read_micros(Chronometer * _this);

#endif /* CHRONOMETER_H_ */

#endif

