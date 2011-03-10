#ifndef __ieee1451_dot5_h
#define __ieee1451_dot5_h

#include <ieee1451_objects.h>
#include <ieee1451_sender_receiver.h>
#include <thread.h>

__BEGIN_SYS


class TEDS_TIM
{
public:
	char *payload;
	unsigned short size;

public:
	TEDS_TIM(char *payload, unsigned short size) : payload(payload), size(size) {};
};

//-------------------------------------------

class IEEE1451Transducer
{
public:
	IEEE1451Transducer();
	virtual ~IEEE1451Transducer() {};

	void receiveMessage(unsigned short transId, const char *message, unsigned int size);

protected:
	virtual TEDS_TIM *getTEDS(char id) = 0;
	//virtual bool running() = 0;
	virtual void start() = 0;
	virtual void stop() = 0;
	virtual void readDataSet(unsigned short transId, unsigned int offset) = 0;
};

//-------------------------------------------

class IEEE1451dot0_TIM
{
private:
	char *metaArray;
	char *timUtnArray;
	char *phyArray;
	TEDS_TIM *META_TEDS;
	TEDS_TIM *TIM_UTN_TEDS;
	TEDS_TIM *PHY_TEDS;

	IEEE1451Transducer *transducer;

	static IEEE1451dot0_TIM *dot0;
	IEEE1451dot0_TIM();

public:
	~IEEE1451dot0_TIM();

	static IEEE1451dot0_TIM *getInstance();

	TEDS_TIM *getTEDS(char id);
	void receiveMessage(unsigned short transId, const char *message, unsigned int size);

	void setTransducer(IEEE1451Transducer *transducer) { this->transducer = transducer; };
};

//-------------------------------------------

class IEEE1451dot5_TIM
{
private:
	SenderReceiver *senderReceiver;
	bool connected;

	static IP::Address NCAP_ADDRESS;

	static IEEE1451dot5_TIM *dot5;
	IEEE1451dot5_TIM();

public:
	~IEEE1451dot5_TIM() { if (senderReceiver) delete senderReceiver; };

	static IEEE1451dot5_TIM *getInstance();

	//bool isConnected() { return conn; };
	void connect();
	void disconnect();
	void sendMessage(unsigned short transId, const char *message, unsigned int length);
	static int messageCallback(unsigned char type, unsigned short transId, const IP::Address &source, const char *message, unsigned int length);
};

__END_SYS

#endif
