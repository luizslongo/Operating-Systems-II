#ifndef __ieee1451_tim_temperature_sensor_h
#define __ieee1451_tim_temperature_sensor_h

#include <ieee1451_tim.h>
#include <mutex.h>
#include <thread.h>

#define DATASET_SIZE	10
#define READ_INTERVAL	550000 //10000000 //microseconds = 10 segundos

__BEGIN_SYS


class IEEE1451TemperatureSensor : public IEEE1451Transducer
{
private:
	unsigned short channelNumber;

	bool tim_im;
	bool polling;

	float dataSet[DATASET_SIZE * sizeof(float)];
	Mutex dataSetMutex;
	int pos;

	//Temperature_Sensor temperature;

	Thread *executeThread;

	char *channelArray;
	char *tempSensorUtnArray;
	TEDS_TIM *CHANNEL_TEDS;
	TEDS_TIM *TEMPSENSOR_UTN_TEDS;

public:
	IEEE1451TemperatureSensor(bool tim_im, bool polling);
	~IEEE1451TemperatureSensor();

protected:
	TEDS_TIM *getTEDS(char id);
	//bool running() { return run; };
	void start();
	void stop();
	void readDataSet(unsigned short transId, unsigned int offset);
	void sendDataSet();

public:
	int execute();
};

__END_SYS

#endif
