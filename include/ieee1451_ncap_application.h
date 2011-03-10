#ifndef __ieee1451_ncap_application_h
#define __ieee1451_ncap_application_h

#include <ieee1451_ncap.h>
#include <ieee1451_objects.h>
#include <utility/list.h>

__BEGIN_SYS


class TEDSRetriever;

class NCAPApplication
{
private:
	friend class TEDSRetriever;

	Simple_List<IEEE1451TIMChannel> cache;
	Simple_List<TEDSRetriever> retrievers;

public:
	NCAPApplication() { IEEE1451dot0_NCAP::getInstance()->setApplication(this); IEEE1451dot5_NCAP::getInstance(); };
	~NCAPApplication();

	IEEE1451TIMChannel *getTIM(const IP::Address &address);
	TEDSRetriever *getRetriever(unsigned short transId);

	void updateTIM(const IP::Address &address);
	void updateTIMCompleted(TEDSRetriever *retriever, IEEE1451TIMChannel *tim, const IP::Address address);

	void reportTimConnected(const IP::Address &address);
	void reportTimDisconnected(const IP::Address &address);
	void reportError(unsigned short transId, int errorCode);
	void reportCommandReply(const IP::Address &address, unsigned short transId, const char *message, unsigned int length);
	void reportTimInitiatedMessage(const IP::Address &address, const char *message, unsigned int length);

	void readTemperature(const IP::Address &address, const char *buffer);

	unsigned short sendOperate(const IP::Address &address, unsigned short channelNumber);
	unsigned short sendIdle(const IP::Address &address, unsigned short channelNumber);
	unsigned short sendReadTEDS(const IP::Address &address, unsigned short channelNumber, char tedsId);
	unsigned short sendReadDataSet(const IP::Address &address, unsigned short channelNumber);

	static int readDataSetThread(NCAPApplication *ncap, IP::Address address, IEEE1451TIMChannel *tim);
};

//-------------------------------------------

class TEDSRetriever
{
private:
	friend class NCAPApplication;

	enum State
	{
		meta_teds					= 0,
		tim_transducer_name_teds	= 1,
		phy_teds					= 2,
		transducer_channel_teds		= 3,
		transducer_name_teds		= 4
	};

	IP::Address address;
	NCAPApplication *app;
	IEEE1451TransducerChannel *transducer;
	IEEE1451TIMChannel *tim;

	short state;
	char tedsId;
	unsigned short lastTransId;

	Simple_List<TEDSRetriever>::Element link;

public:
	TEDSRetriever(const IP::Address &address, NCAPApplication *app);
	~TEDSRetriever() {};

	void repeat() { execute(); };
	void process(const char *message, unsigned int length);
	void execute();
};

__END_SYS

#endif
