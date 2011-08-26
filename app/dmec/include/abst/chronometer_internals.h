/*
 * chronometer_internals.h
 *
 *  Created on: Feb 4, 2011
 *      Author: mateus
 */

#if 0

#ifndef CHRONOMETER_INTERNALS_H_
#define CHRONOMETER_INTERNALS_H_

#include <sys/time.h>
#include <time.h>


typedef long long Second;
typedef long long Milisecond;
typedef long long Microsecond;


typedef
struct _Chronometer {
	Second __initialTimeSecondComponent;
	Second __finalTimeSecondComponent;
	Second __elapsedTimeSecondComponent;

    Microsecond __initialTimeMicroComponent;
    Microsecond __finalTimeMicroComponent;
    Microsecond __elapsedTimeMicroComponent;

    struct timeval __timeStruct;

} Chronometer;


#endif /* CHRONOMETER_INTERNALS_H_ */

#endif
