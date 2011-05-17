#ifndef __ieee1451_ncap_application_h
#define __ieee1451_ncap_application_h

#define USE_SIP

#include <ieee1451_ncap.h>
#include <ieee1451_objects.h>
#include <utility/list.h>
#ifdef USE_SIP
	#include <sip_defs.h>
	#include <sip_manager.h>
	#include <sip_user_agent.h>
#endif

__BEGIN_SYS


class TEDSRetriever;

class NCAPApplication
{
private:
	friend class TEDSRetriever;

	struct TIMCache
	{
		IEEE1451TIMChannel *tim;
		Simple_List<TIMCache>::Element link;

#ifdef USE_SIP
		UserAgent *ua;

		TIMCache(IEEE1451TIMChannel *tim, UserAgent *ua) : tim(tim), ua(ua), link(this) {};
		~TIMCache() { delete tim; delete ua; };
#else
		TIMCache(IEEE1451TIMChannel *tim) : tim(tim), link(this) {};
		~TIMCache() { delete tim; };
#endif
	};

	Simple_List<TIMCache> cache;
	Simple_List<TEDSRetriever> retrievers;

	static NCAPApplication *app;
	NCAPApplication();

public:
	~NCAPApplication();

	static NCAPApplication *getInstance();

	TIMCache *getTIMCache(const IP::Address &address);
#ifdef USE_SIP
	TIMCache *getTIMCache(const char *uri);
#endif
	TEDSRetriever *getRetriever(unsigned short transId);

	void updateTIM(const IP::Address &address);
	void updateTIMCompleted(TEDSRetriever *retriever, IEEE1451TIMChannel *tim, IP::Address address);

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

#ifdef USE_SIP
	void sendSipMessage(UserAgent *ua, const char *data);
	void sendSipNotify(UserAgent *ua, SipSubscriptionState state, SipPidfXmlBasicElement pidfXml);
	static int messageCallback(SipEventCallback event, UserAgent *ua, const char *remote);
#endif

	//static int readDataSetThread(NCAPApplication *ncap, IP::Address address, IEEE1451TIMChannel *tim);
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
