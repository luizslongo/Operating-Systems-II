#ifndef __sip_user_agent_h
#define __sip_user_agent_h

#include <sip_defs.h>
#include <sip_header.h>
#include <sip_message.h>
#include <sip_transactions.h>
#include <utility/list.h>

__BEGIN_SYS


class SipDialog
{
private:
	friend class UserAgent;
	friend class UserAgentClient;
	friend class UserAgentServer;

	enum State
	{
		sttIdle,
		//sttEarly,
		sttConfirmed
	} curState;

	char *callID;
	char *localTag;
	char *remoteTag;
	unsigned int localSequenceNumber;
	unsigned int remoteSequenceNumber;
	char *localURI;
	char *remoteURI;
	char *remoteTarget;
	Simple_List<SipHeader/*Route*/> routeSet;

public:
	SipDialog();
	~SipDialog() { clear(); };

	void setDialog(const char *callID, const char *localTag, const char *remoteTag,
			unsigned int localSequenceNumber, unsigned int remoteSequenceNumber,
			const char *localURI, const char *remoteURI, const char *remoteTarget);
	void setRemoteTarget(const char *remoteTarget);
	void addRouteBack(SipHeaderRoute *route);
	void addRouteFront(SipHeaderRoute *route);
	void clearRouteSet();

	int getRouteSetSize() { return (int) routeSet.size(); };
	SipHeaderRoute *getRoute(int pos);

	bool isActive() { return curState != sttIdle; };
	void clear();
};

//-------------------------------------------

class SipSubscription
{
private:
	friend class UserAgent;
	friend class UserAgentClient;
	friend class UserAgentServer;

	SipEventPackage eventType;
	char *eventId;

	//Functor_Handler *timerHandler;
	//Alarm *timerAlarm;

public:
	SipSubscription() : eventType(SIP_EVENT_PACKAGE_INVALID), eventId(0)/*, timerHandler(0), timerAlarm(0)*/ {};
	~SipSubscription() { clear(); };

	void setSubscription(SipEventPackage eventType, const char *eventId);
	//void startTimer(void *p);
	//void stopTimer();

	bool isActive() { return eventType != SIP_EVENT_PACKAGE_INVALID; };
	void clear();
};

//-------------------------------------------

class UserAgentClient
{
private:
	friend class UserAgent;
	UserAgent *ua;

public:
	UserAgentClient(UserAgent *ua) : ua(ua) {};
	~UserAgentClient() {};

private:
	SipRequest *createRequest(SipMessageType msgType, const char *to = 0, SipMessage *invite = 0, SipSubscriptionState state = SIP_SUBSCRIPTION_STATE_INVALID, SipPidfXmlBasicElement pidfXmlElement = SIP_PIDF_XML_BASIC_ELEMENT_INVALID, unsigned int expires = 0, const char *data = 0);

public:
	SipRequestAck *createAck(SipRequestInvite *invite) { return (SipRequestAck *) createRequest(SIP_REQUEST_ACK, 0, invite); };
	SipRequestBye *createBye() { return (SipRequestBye *) createRequest(SIP_REQUEST_BYE); };
	SipRequestInvite *createInvite(const char *to = 0) { return (SipRequestInvite *) createRequest(SIP_REQUEST_INVITE, to); };
	SipRequestMessage *createMessage(const char *data) { return (SipRequestMessage *) createRequest(SIP_REQUEST_MESSAGE, 0, 0, SIP_SUBSCRIPTION_STATE_INVALID, SIP_PIDF_XML_BASIC_ELEMENT_INVALID, 0, data); };
	SipRequestNotify *createNotify(SipSubscriptionState state, SipPidfXmlBasicElement pidfXmlElement, unsigned int expires = 0) { return (SipRequestNotify *) createRequest(SIP_REQUEST_NOTIFY, 0, 0, state, pidfXmlElement, expires); };
	SipRequestSubscribe *createSubscribe() { return 0; };
	void sendRequest(SipRequest *request);

	bool receiveMsg(SipResponse *response);
	bool receiveMsg(SipRequest *request, SipResponse *response, Transaction *transaction);
	//bool receive1xx(SipRequest *request, SipResponse *response, Transaction *transaction) { return true; };
	bool receive2xx(SipRequest *request, SipResponse *response, Transaction *transaction);
	bool receive3xx6xx(SipRequest *request, SipResponse *response, Transaction *transaction);

	bool createDialog(SipRequest *request, SipResponse *response);
};

//-------------------------------------------

class UserAgentServer
{
private:
	friend class UserAgent;
	UserAgent *ua;

public:
	UserAgentServer(UserAgent *ua) : ua(ua) {}
	~UserAgentServer() {};

private:
	SipResponse *createResponse(int statusCode, SipRequest *request);
	void sendResponse(SipResponse *response, SipMessageType requestType, Transaction *transaction);

public:
	bool receiveMsg(SipRequest *request);
	bool receiveMsg(SipRequest *request, Transaction *transaction);
	//bool receiveAck(SipRequestAck *request, Transaction *transaction) { return true; };
	bool receiveBye(SipRequestBye *request, Transaction *transaction);
	bool receiveInvite(SipRequestInvite *request, Transaction *transaction);
	bool receiveMessage(SipRequestMessage *request, Transaction *transaction);
	//bool receiveNotify(SipRequestNotify *request, Transaction *transaction) { return true; };
	bool receiveSubscribe(SipRequestSubscribe *request, Transaction *transaction);

	bool createDialog(SipRequest *request, SipResponse *response);
};

//-------------------------------------------

class UserAgent
{
private:
	friend class UserAgentClient;
	friend class UserAgentServer;
	friend class TransactionClientInvite;
	friend class TransactionClientNonInvite;
	friend class TransactionServerInvite;
	friend class TransactionServerNonInvite;
	friend class SipManager;

	UserAgentClient uac;
	UserAgentServer uas;

	SipDialog dialog;
	SipSubscription subscription;
	Simple_List<Transaction> transactions;

	char *uri;
	const char *textReceived;

	int timerValues[SIP_TIMER_COUNT];
	Functor_Handler<Transaction> *timerHandlers[SIP_TIMER_COUNT];
	Alarm *timerAlarms[SIP_TIMER_COUNT];

	Simple_List<UserAgent>::Element link;

public:
	UserAgent(const char *uri);
	~UserAgent();

	UserAgentClient *getUserAgentClient() { return &uac; };
	UserAgentServer *getUserAgentServer() { return &uas; };

	//bool matchingDialog(SipMessage *msg);
	Transaction *matchingTransaction(SipMessage *msg);

	const char *getUri() { return uri; };
	const char *getTextReceived() { return textReceived; };
	bool hasSubscription() { return subscription.isActive(); };

	void addTransaction(Transaction *transaction) { transactions.insert(&transaction->link); };
	void removeTransaction(Transaction *transaction) { transactions.remove(&transaction->link); delete transaction; };

	int getTimerValue(SipTimer timer) { return timerValues[timer]; };
	void setTimerValue(SipTimer timer, int timerValue) { timerValues[timer] = timerValue; };
	void startTimer(SipTimer timer, Transaction *p);
	void stopTimer(SipTimer timer);
};

//-------------------------------------------

class SendRTP
{
private:
	unsigned int sequence;
	unsigned int timestamp;

	UDP udp;

public:
	SendRTP() : sequence(0x016a), timestamp(0x00207a10), udp(IP::instance()), socket(&udp) {};
	~SendRTP() {};

	void sendData(const char *destination, unsigned short port, char *data, unsigned int size);

private:
	class MySocket : public UDP::Socket
	{
	public:
		MySocket(UDP *udp) : UDP::Socket(udp, UDP::Address(IP::instance()->address(), 8000), UDP::Address(Traits<IP>::BROADCAST, 8000)) {};
		virtual ~MySocket() {};

		void received(const UDP::Address &src, const char *data, unsigned int size)
		{
			db<SendRTP::MySocket>(INF) << "SendRTP::MySocket::received..\n";
		};
	} socket;
};

__END_SYS

#endif
