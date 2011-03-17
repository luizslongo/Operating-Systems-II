#ifndef __sip_transactions_h
#define __sip_transactions_h

#include <sip_defs.h>
#include <sip_manager.h>
#include <sip_message.h>

__BEGIN_SYS


class UserAgentClient;
class UserAgentServer;

class Transaction
{
private:
	friend class UserAgent;

protected:
	SipTransportLayer *transport;
	UserAgent *ua;

	Simple_List<Transaction>::Element link;

public:
	Transaction(UserAgent *ua);
	virtual ~Transaction() {};

	virtual SipTransactionType getTransactionType() = 0;
	virtual Transaction *matchingTransaction(SipMessage *msg) = 0;
	void clear();
};

//-------------------------------------------

class TransactionClientInvite : public Transaction
{
private:
	enum State
	{
		sttIdle,
		sttCalling,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequestInvite *requestMsg;

public:
	TransactionClientInvite(UserAgent *ua);
	~TransactionClientInvite();

	void sendAck(SipResponse *Msg);

	void sendInvite(SipRequestInvite *Msg);
	void receive1xx(SipResponse *Msg);
	void receive2xx(SipResponse *Msg);
	void receive3xx6xx(SipResponse *Msg);
	void transportError();

	void timerAExpired();
	void timerBExpired();
	void timerDExpired();

	static void timerACallback(Transaction *p) { ((TransactionClientInvite *) p)->timerAExpired(); };
	static void timerBCallback(Transaction *p) { ((TransactionClientInvite *) p)->timerBExpired(); };
	static void timerDCallback(Transaction *p) { ((TransactionClientInvite *) p)->timerDExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_CLIENT_INVITE; };
	Transaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class TransactionClientNonInvite : public Transaction
{
private:
	enum State
	{
		sttIdle,
		sttTrying,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequest *requestMsg;

public:
	TransactionClientNonInvite(UserAgent *ua);
	~TransactionClientNonInvite();

	//void copyRequest(SipRequest *Msg);

	void sendRequest(SipRequest *Msg);
	void receive1xx(SipResponse *Msg);
	void receive2xx6xx(SipResponse *Msg);
	void transportError();

	void timerEExpired();
	void timerFExpired();
	void timerKExpired();

	static void timerECallback(Transaction *p) { ((TransactionClientNonInvite *) p)->timerEExpired(); };
	static void timerFCallback(Transaction *p) { ((TransactionClientNonInvite *) p)->timerFExpired(); };
	static void timerKCallback(Transaction *p) { ((TransactionClientNonInvite *) p)->timerKExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_CLIENT_NON_INVITE; };
	Transaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class TransactionServerInvite : public Transaction
{
private:
	enum State
	{
		sttIdle,
		sttProceeding,
		sttCompleted,
		sttConfirmed,
		//sttTerminated
	} curState;

	SipRequestInvite *requestMsg;
	SipResponse *lastResponse;

public:
	TransactionServerInvite(UserAgent *ua);
	~TransactionServerInvite();

	void receiveInvite(SipRequestInvite *Msg);
	void receiveAck(SipRequestAck *Msg);
	void send1xx(SipResponse *Msg);
	void send2xx(SipResponse *Msg);
	void send3xx6xx(SipResponse *Msg);
	void transportError();

	void timerGExpired();
	void timerHExpired();
	void timerIExpired();

	static void timerGCallback(Transaction *p) { ((TransactionServerInvite *) p)->timerGExpired(); };
	static void timerHCallback(Transaction *p) { ((TransactionServerInvite *) p)->timerHExpired(); };
	static void timerICallback(Transaction *p) { ((TransactionServerInvite *) p)->timerIExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_SERVER_INVITE; };
	Transaction *matchingTransaction(SipMessage *msg);
};

//-------------------------------------------

class TransactionServerNonInvite : public Transaction
{
private:
	enum State
	{
		sttIdle,
		sttTrying,
		sttProceeding,
		sttCompleted,
		//sttTerminated
	} curState;

	SipRequest *requestMsg;
	SipResponse *lastResponse;

public:
	TransactionServerNonInvite(UserAgent *ua);
	~TransactionServerNonInvite();

	//void copyRequest(SipRequest *Msg);

	void receiveRequest(SipRequest *Msg);
	void send1xx(SipResponse *Msg);
	void send2xx6xx(SipResponse *Msg);
	void transportError();

	void timerJExpired();

	static void timerJCallback(Transaction *p) { ((TransactionServerNonInvite *) p)->timerJExpired(); };

	SipTransactionType getTransactionType() { return SIP_TRANSACTION_SERVER_NON_INVITE; };
	Transaction *matchingTransaction(SipMessage *msg);
};

__END_SYS

#endif
