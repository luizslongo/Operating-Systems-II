/*
 * chronometer.c
 *
 *  Created on: Feb 4, 2011
 *      Author: mateus
 */

#if 0

#include <dmec_java/abst/chronometer.h>


static inline void __computeCurrentTime(Chronometer * _this);


void setUpChronometer(Chronometer * _this)
{
	chrono_clear(_this);
}


void chrono_clear(Chronometer * _this)
{
	_this->__initialTimeSecondComponent = 0;
	_this->__initialTimeMicroComponent = 0;

	_this->__finalTimeSecondComponent = 0;
	_this->__finalTimeMicroComponent = 0;

	_this->__elapsedTimeSecondComponent = 0;
	_this->__elapsedTimeMicroComponent = 0;
}


void chrono_start(Chronometer * _this)
{
    __computeCurrentTime(_this);
    _this->__initialTimeSecondComponent = (Second) _this->__timeStruct.tv_sec;
    _this->__initialTimeMicroComponent = (Microsecond) _this->__timeStruct.tv_usec;
}


void chrono_pause(Chronometer * _this)
{
    __computeCurrentTime(_this);
    _this->__finalTimeSecondComponent = (Second) _this->__timeStruct.tv_sec;
    _this->__finalTimeMicroComponent = (Microsecond) _this->__timeStruct.tv_usec;

	_this->__elapsedTimeSecondComponent +=
	        _this->__finalTimeSecondComponent - _this->__initialTimeSecondComponent;

	_this->__elapsedTimeMicroComponent +=
	        _this->__finalTimeMicroComponent - _this->__initialTimeMicroComponent;
}


Second chrono_read_seconds(Chronometer * _this)
{
	return _this->__elapsedTimeSecondComponent; /* ignoring the microsecond component */
}


Milisecond chrono_read_milis(Chronometer * _this)
{
    return (_this->__elapsedTimeSecondComponent * 1000) +
            (_this->__elapsedTimeMicroComponent / 1000);
}


Microsecond chrono_read_micros(Chronometer * _this)
{
    return (_this->__elapsedTimeSecondComponent * 1000000) +
            (_this->__elapsedTimeMicroComponent);
}

void __computeCurrentTime(Chronometer * _this)
{
	gettimeofday(&(_this->__timeStruct), NULL);
}

#endif
