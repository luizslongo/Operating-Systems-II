
#include <sip_user_agent.h>

__BEGIN_SYS


SipDialog::SipDialog()
{
	curState = sttIdle;

	callID = 0;
	localTag = 0;
	remoteTag = 0;
	localSequenceNumber = 0;
	remoteSequenceNumber = 0;
	localURI = 0;
	remoteURI = 0;
	remoteTarget = 0;
}

void SipDialog::setDialog(const char *callID, const char *localTag, const char *remoteTag,
				unsigned int localSequenceNumber, unsigned int remoteSequenceNumber,
				const char *localURI, const char *remoteURI, const char *remoteTarget)
{
	if (this->callID) delete this->callID;
	this->callID = createString(callID);

	if (this->localTag) delete this->localTag;
	this->localTag = createString(localTag);

	if (this->remoteTag) delete this->remoteTag;
	this->remoteTag = createString(remoteTag);

	this->localSequenceNumber = localSequenceNumber;

	this->remoteSequenceNumber = remoteSequenceNumber;

	if (this->localURI) delete this->localURI;
	this->localURI = createString(localURI);

	if (this->remoteURI) delete this->remoteURI;
	this->remoteURI = createString(remoteURI);

	if (this->remoteTarget) delete this->remoteTarget;
	this->remoteTarget = createString(remoteTarget);

	this->curState = sttConfirmed;
}

void SipDialog::setRemoteTarget(const char *remoteTarget)
{
	if (this->remoteTarget) delete this->remoteTarget;
	this->remoteTarget = createString(remoteTarget);
}

void SipDialog::addRouteBack(SipHeaderRoute *route)
{
	SipHeaderRoute *route2 = new SipHeaderRoute(*route);
	routeSet.insert_tail(&route2->link);
}

void SipDialog::addRouteFront(SipHeaderRoute *route)
{
	SipHeaderRoute *route2 = new SipHeaderRoute(*route);
	routeSet.insert_head(&route2->link);
}

void SipDialog::clearRouteSet()
{
	Simple_List<SipHeader/*Route*/>::Iterator it = routeSet.begin();
	while (it != routeSet.end())
	{
		SipHeaderRoute *route = (SipHeaderRoute *) it->object();
		routeSet.remove(it++);
		delete route;
	}
}

SipHeaderRoute *SipDialog::getRoute(int pos)
{
	int i = 0;
	Simple_List<SipHeader/*Route*/>::Iterator it = routeSet.begin();
	while (it != routeSet.end())
	{
		if (i == pos)
			return (SipHeaderRoute *) it->object();
		i++; it++;
	}
	return 0;
}

void SipDialog::clear()
{
	curState = sttIdle;

	if (callID)
		delete callID;
	callID = 0;

	if (localTag)
		delete localTag;
	localTag = 0;

	if (remoteTag)
		delete remoteTag;
	remoteTag = 0;

	localSequenceNumber = 0;

	remoteSequenceNumber = 0;

	if (localURI)
		delete localURI;
	localURI = 0;

	if (remoteURI)
		delete remoteURI;
	remoteURI = 0;

	if (remoteTarget)
		delete remoteTarget;
	remoteTarget = 0;

	clearRouteSet();
}

//-------------------------------------------

void SipSubscription::setSubscription(SipEventPackage eventType, const char *eventId)
{
	this->eventType = eventType;

	if (this->eventId) delete this->eventId;
	this->eventId = createString(eventId);
}

/*void SipSubscription::startTimer(void *p)
{
	timerHandler = new Functor_Handler(&TransactionClientNonInvite::timerKCallback, p);
	timerAlarm = new Alarm(timerValue, timerHandler, 1);
}

void SipSubscription::stopTimer()
{
	if (timerHandler)
	{
		delete timerHandler;
		timerHandler = 0;
	}

	if (timerAlarm)
	{
		delete timerAlarm;
		timerAlarm = 0;
	}
}*/

void SipSubscription::clear()
{
	eventType = SIP_EVENT_PACKAGE_INVALID;

	if (eventId)
		delete eventId;
	eventId = 0;

	//stopTimer();
}

//-------------------------------------------

SipRequest *UserAgentClient::createRequest(SipMessageType msgType, const char *to, SipMessage *invite,
		SipSubscriptionState state, SipPidfXmlBasicElement pidfXmlElement, unsigned int expires,
		const char *data)
{
	if (((!ua->dialog.isActive()) && (!to)) ||
		(((msgType == SIP_REQUEST_ACK) /*|| (msgType == SIP_REQUEST_CANCEL)*/) && (!invite)) ||
		((msgType == SIP_REQUEST_NOTIFY) && (!ua->subscription.isActive())) ||
		((msgType == SIP_REQUEST_MESSAGE) && (!data)))
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createRequest -> Invalid parameters\n";
		return 0;
	}

	SipRequest *request = 0;

	switch (msgType)
	{
		case SIP_REQUEST_ACK:			request = new SipRequestAck();			break;
		case SIP_REQUEST_BYE:			request = new SipRequestBye();			break;
		//case SIP_REQUEST_CANCEL:		request = new SipRequestCancel();		break;
		case SIP_REQUEST_INVITE:		request = new SipRequestInvite();		break;
		case SIP_REQUEST_MESSAGE:		request = new SipRequestMessage();		break;
		case SIP_REQUEST_NOTIFY:		request = new SipRequestNotify();		break;
		//case SIP_REQUEST_OPTIONS:		request = new SipRequestOptions();		break;
		//case SIP_REQUEST_REGISTER:	request = new SipRequestRegister();		break;
		case SIP_REQUEST_SUBSCRIBE:		request = new SipRequestSubscribe();	break;
		//case SIP_RESPONSE:			break;
		default:						break;
	}

	if (!request)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::createRequest -> Failed to create the request (message type = " << msgType << ")\n";
		return 0;
	}

	SipHeaderVia *headerVia = new SipHeaderVia();
	char branch[20];
	strcpy(branch, "z9hG4bK");
	SipManager::random(&branch[7]);
	headerVia->setVia("SIP", "2.0", SIP_TRANSPORT_UDP, SipManager::getInstance()->getTransport()->getHostIP(), SipManager::getInstance()->getTransport()->getHostPort(), branch);
	request->addHeader(headerVia);

	SipHeaderTo *headerTo = new SipHeaderTo();
	request->addHeader(headerTo);

	SipHeaderFrom *headerFrom = new SipHeaderFrom();
	request->addHeader(headerFrom);

	SipHeaderCallID *headerCallID = new SipHeaderCallID();
	request->addHeader(headerCallID);

	SipHeaderCSeq *headerCSeq = new SipHeaderCSeq();
	request->addHeader(headerCSeq);

	SipHeaderMaxForwards *headerMaxForwards = new SipHeaderMaxForwards();
	headerMaxForwards->setNumber(70);
	request->addHeader(headerMaxForwards);

	SipHeaderContact *headerContact = new SipHeaderContact();
	request->addHeader(headerContact);


	if (!ua->dialog.isActive())
	{
		headerTo->setAddress(to);

		char tag[20];
		SipManager::random(tag);
		headerFrom->setAddress(ua->uri);
		headerFrom->setTag(tag);

		char callid[100], host[255], aux[255];
		//static int localidResgister = 2900;
		strcpy(host, ua->uri);
		match(host, ":" , aux);
		skip(host, " \t");
		match(host, "@" , aux);
		skip(host, " \t");
		//if (msgType == SIP_REQUEST_REGISTER)
		//	itoa(localidResgister, callid);
		//else
			SipManager::random(callid);
		strcat(callid, "@");
		strcat(callid, host);
		headerCallID->setString(callid);

		static unsigned int seq = 1;
		headerCSeq->setCSeq(msgType, seq++);

		headerContact->setAddress(ua->uri);

		request->setRequestLine(msgType, to, SIP_VERSION);
	}else
	{
		headerTo->setAddress(ua->dialog.remoteURI);
		if (ua->dialog.remoteTag)
			headerTo->setTag(ua->dialog.remoteTag);

		headerFrom->setAddress(ua->dialog.localURI);
		if (ua->dialog.localTag)
			headerFrom->setTag(ua->dialog.localTag);

		headerCallID->setString(ua->dialog.callID);

		if (ua->dialog.localSequenceNumber == 0)
			ua->dialog.localSequenceNumber = 1;
		unsigned int seq = 0;
		if ((msgType == SIP_REQUEST_ACK)) //|| (msgType == SIP_REQUEST_CANCEL))
		{
			SipHeaderCSeq *cseq = (SipHeaderCSeq *) invite->getHeader(SIP_HEADER_CSEQ);
			seq = cseq->getSequence();
		}else
		{
			seq = ua->dialog.localSequenceNumber + 1;
			ua->dialog.localSequenceNumber = seq;
		}
		headerCSeq->setCSeq(msgType, seq);

		headerContact->setAddress(ua->dialog.localURI);

		const char *remote = 0;
		int routeSetSize = ua->dialog.getRouteSetSize();
		if (routeSetSize == 0)
			remote = ua->dialog.remoteTarget;
		else if (routeSetSize > 0)
		{
			SipHeaderRoute *route = ua->dialog.getRoute(0);
			if (route->isLR())
			{
				remote = ua->dialog.remoteTarget;

				for (int i = 0; i < routeSetSize; i++)
					request->addHeader(new SipHeaderRoute(*(ua->dialog.getRoute(i))));
			}else
			{
				remote = ua->dialog.getRoute(0)->getAddress();

				for (int i = 1; i < routeSetSize; i++)
					request->addHeader(new SipHeaderRoute(*(ua->dialog.getRoute(i))));

				SipHeaderRoute *header = new SipHeaderRoute();
				header->setAddress(ua->dialog.remoteTarget, true);
				request->addHeader(header);
			}
		}

		char pRemote[512];
		strcpy(pRemote, remote);
		int size = strlen(pRemote);
		for (int i = size - 1; i >= size - 6; i--)
		{
			if (pRemote[i] == ':')
			{
				pRemote[i] = 0;
				break;
			}
		}
		request->setRequestLine(msgType, pRemote, SIP_VERSION);
	}

	if (msgType == SIP_REQUEST_INVITE)
	{
		SipHeaderAllow *headerAllow = new SipHeaderAllow();
		for (int i = 0; i < (SIP_MESSAGE_TYPE_INVALID - 1); i++)
			headerAllow->addAllowed((SipMessageType) i);
		request->addHeader(headerAllow);

		SipHeaderContentDisposition *headerContentDisposition = new SipHeaderContentDisposition();
		headerContentDisposition->setString("session");
		request->addHeader(headerContentDisposition);

		SipSdpBody *sdp = new SipSdpBody();
		request->setBody(sdp);

	}else if (msgType == SIP_REQUEST_NOTIFY)
	{
		SipHeaderEvent *headerEvent = new SipHeaderEvent();
		headerEvent->setEvent(ua->subscription.eventType, ua->subscription.eventId);
		request->addHeader(headerEvent);

		SipHeaderSubscriptionState *headerSubscriptionState = new SipHeaderSubscriptionState();
		headerSubscriptionState->setSubscriptionState(state, expires);
		request->addHeader(headerSubscriptionState);

		SipPidfXmlBody *pidf = new SipPidfXmlBody();
		pidf->setElement(pidfXmlElement);
		request->setBody(pidf);
	}else if (msgType == SIP_REQUEST_MESSAGE)
	{
		SipTextPlainBody *text = new SipTextPlainBody();
		text->setText(data);
		request->setBody(text);
	}

	return request;
}

void UserAgentClient::sendRequest(SipRequest *request)
{
	if (!request)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::sendRequest -> Invalid parameter\n";
		return;
	}

	if (request->getMsgType() == SIP_REQUEST_INVITE)
	{
		TransactionClientInvite *transaction = new TransactionClientInvite(ua);
		ua->addTransaction(transaction);
		transaction->sendInvite((SipRequestInvite *) request);
	}else
	{
		TransactionClientNonInvite *transaction = new TransactionClientNonInvite(ua);
		ua->addTransaction(transaction);
		transaction->sendRequest(request);
	}

	if (request->getCanDelete())
		delete request;
}

bool UserAgentClient::receiveMsg(SipResponse *response)
{
	if (response->getNumHeader(SIP_HEADER_VIA) != 1)
		return false;

	int statusCode = response->getStatusLine()->getStatusCode();
	Transaction *transaction = ua->matchingTransaction(response);

	if (!transaction)
	{
		db<UserAgentClient>(WRN) << "UserAgentClient::receiveMsg -> Ignoring invalid response\n";
		return false;
	}

	if (transaction->getTransactionType() == SIP_TRANSACTION_CLIENT_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((TransactionClientInvite *) transaction)->receive1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 299))
			((TransactionClientInvite *) transaction)->receive2xx(response);

		else if ((statusCode >= 300) && (statusCode <= 699))
			((TransactionClientInvite *) transaction)->receive3xx6xx(response);

	}else if (transaction->getTransactionType() == SIP_TRANSACTION_CLIENT_NON_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((TransactionClientNonInvite *) transaction)->receive1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 699))
			((TransactionClientNonInvite *) transaction)->receive2xx6xx(response);
	}

	return true;
}

bool UserAgentClient::receiveMsg(SipRequest *request, SipResponse *response, Transaction *transaction)
{
	int statusCode = response->getStatusLine()->getStatusCode();

	if ((statusCode >= 200) && (statusCode <= 299))
		receive2xx(request, response, transaction);

	else if ((statusCode >= 300) && (statusCode <= 699))
		receive3xx6xx(request, response, transaction);

	return true;
}

bool UserAgentClient::receive2xx(SipRequest *request, SipResponse *response, Transaction *transaction)
{
	switch (request->getMsgType())
	{
		case SIP_REQUEST_INVITE:
		{
			if (!ua->dialog.isActive())
			{
				createDialog(request, response);

				SipRequestAck *ack = createAck((SipRequestInvite *) request);
				SipManager::getInstance()->getTransport()->sendMessage(ack);
				delete ack;
			}else
			{
				SipHeaderContact *contact = (SipHeaderContact *) response->getHeader(SIP_HEADER_CONTACT);
				if (contact)
				{
					const char *target = contact->getAddress();
					if (target)
						ua->dialog.setRemoteTarget(target);
				}
			}
			break;
		}

		case SIP_REQUEST_BYE:
		{
			if (ua->dialog.isActive())
			{
				ua->dialog.clear();
				SipManager::callback(SIP_SESSION_TERMINATED, ua);
			}
			break;
		}

		default:
			break;
	}

	return true;
}

bool UserAgentClient::receive3xx6xx(SipRequest *request, SipResponse *response, Transaction *transaction)
{
	int statusCode = response->getStatusLine()->getStatusCode();

	if ((statusCode == 408) || (statusCode == 481) || (request->getMsgType() == SIP_REQUEST_BYE))
	{
		if (ua->dialog.isActive())
		{
			ua->dialog.clear();
			SipManager::callback(SIP_SESSION_TERMINATED, ua);
		}
		return true;
	}

	if (ua->dialog.isActive())
	{
		SipRequestBye *bye = createBye();
		sendRequest(bye);
		//delete bye;

		ua->dialog.clear();
		SipManager::callback(SIP_SESSION_TERMINATED, ua);
	}

	return true;
}

bool UserAgentClient::createDialog(SipRequest *request, SipResponse *response)
{
	SipHeaderFrom *fromRequest = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderFrom *fromResponse = (SipHeaderFrom *) response->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderContact *contact = (SipHeaderContact *) response->getHeader(SIP_HEADER_CONTACT);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderTo *to = (SipHeaderTo *) response->getHeader(SIP_HEADER_TO);

	if ((!fromRequest) || (!fromResponse) || (!callId) || (!contact) || (!cseq) || (!to))
		return false;

	const char *id = callId->getString();
	const char *localTag = fromRequest->getTag();
	const char *remoteTag = to->getTag(); //Pode ser NULL, compatibilidade com RFC 2543
	const char *localURI = fromResponse->getAddress();
	const char *remoteURI = to->getAddress();
	const char *target = contact->getAddress();
	unsigned int sequenceNumber = cseq->getSequence();

	if ((!id) || (!localTag) || (!localURI) || (!remoteURI) || (!target))
		return false;

	ua->dialog.setDialog(id, localTag, remoteTag, sequenceNumber, 0, localURI, remoteURI, target);

	int recordRouteNum = response->getNumHeader(SIP_HEADER_RECORD_ROUTE);
	for (int i = 0; i < recordRouteNum; i++)
	{
		SipHeaderRoute *route = (SipHeaderRoute *) response->getHeader(SIP_HEADER_RECORD_ROUTE, i);
		ua->dialog.addRouteFront(route);
	}

	SipManager::callback(SIP_SESSION_INITIATED, ua);
	return true;
}

//-------------------------------------------

SipResponse *UserAgentServer::createResponse(int statusCode, SipRequest *request)
{
	if ((statusCode < 100) || (statusCode > 699) || (!request))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::createResponse -> Invalid parameters\n";
		return 0;
	}

	int viaNum = request->getNumHeader(SIP_HEADER_VIA);
	SipHeaderFrom *from = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderTo *to = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);

	if ((viaNum == 0) || (!from) || (!callId) || (!cseq) || (!to))
		return 0;

	SipResponse *response = new SipResponse(statusCode);

	for (int i = 0; i < viaNum; i++)
	{
		SipHeaderVia *via = (SipHeaderVia *) request->getHeader(SIP_HEADER_VIA, i);
		response->addHeader(new SipHeaderVia(*via));
	}

	SipHeaderTo *headerTo = new SipHeaderTo(*to);
	response->addHeader(headerTo);
	const char *toTag = headerTo->getTag();
	if ((!toTag) && (statusCode != 100))
	{
		char tag[20];
		SipManager::random(tag);
		headerTo->setTag(tag);
	}

	response->addHeader(new SipHeaderFrom(*from));
	response->addHeader(new SipHeaderCallID(*callId));
	response->addHeader(new SipHeaderCSeq(*cseq));

	SipHeaderMaxForwards *headerMaxForwards = new SipHeaderMaxForwards();
	headerMaxForwards->setNumber(70);
	response->addHeader(headerMaxForwards);

	if ((request->getMsgType() == SIP_REQUEST_INVITE) || (request->getMsgType() == SIP_REQUEST_SUBSCRIBE))
	{
		SipHeaderContact *headerContact = new SipHeaderContact();
		const char *address = headerTo->getAddress();
		headerContact->setAddress(address);
		response->addHeader(headerContact);
	}

	if (request->getMsgType() == SIP_REQUEST_INVITE)
	{
		SipHeaderContentDisposition *headerContentDisposition = new SipHeaderContentDisposition();
		headerContentDisposition->setString("session");
		response->addHeader(headerContentDisposition);

		SipSdpBody *sdp = new SipSdpBody();
		response->setBody(sdp);
	}

	return response;
}

void UserAgentServer::sendResponse(SipResponse *response, SipMessageType requestType, Transaction *transaction)
{
	if ((!response) || (!transaction))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::sendResponse -> Invalid parameters\n";
		return;
	}

	int statusCode = response->getStatusLine()->getStatusCode();

	if (requestType == SIP_REQUEST_INVITE)
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((TransactionServerInvite *) transaction)->send1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 299))
			((TransactionServerInvite *) transaction)->send2xx(response);

		else if ((statusCode >= 300) && (statusCode <= 699))
			((TransactionServerInvite *) transaction)->send3xx6xx(response);
	}else
	{
		if ((statusCode >= 100) && (statusCode <= 199))
			((TransactionServerNonInvite *) transaction)->send1xx(response);

		else if ((statusCode >= 200) && (statusCode <= 699))
			((TransactionServerNonInvite *) transaction)->send2xx6xx(response);
	}

	if (response->getCanDelete())
		delete response;
}

bool UserAgentServer::receiveMsg(SipRequest *request)
{
	SipMessageType requestType = request->getMsgType();
	Transaction *transaction = ua->matchingTransaction(request);

	if ((!transaction) && (requestType == SIP_REQUEST_ACK))
	{
		db<UserAgentServer>(WRN) << "UserAgentServer::receiveMsg -> Invalid ACK message\n";
		return false;

	}else if ((!transaction) && (requestType == SIP_REQUEST_INVITE))
	{
		if (ua->dialog.isActive()) //TODO: Se já existe um dialog, não recebe mais INVITE!
		{
			db<UserAgentServer>(WRN) << "UserAgentServer::receiveMsg -> There is already a dialog\n";
			return false;
		}

		transaction = new TransactionServerInvite(ua);
		ua->addTransaction(transaction);

	}else if (!transaction) //&& (requestType != SIP_REQUEST_INVITE) && (requestType != SIP_REQUEST_ACK))
	{
		transaction = new TransactionServerNonInvite(ua);
		ua->addTransaction(transaction);
	}

	db<UserAgentServer>(INF) << "UserAgentServer::receiveMsg -> New request received\n";

	if (requestType == SIP_REQUEST_INVITE)
		((TransactionServerInvite *) transaction)->receiveInvite((SipRequestInvite *) request);

	else if (requestType == SIP_REQUEST_ACK)
		((TransactionServerInvite *) transaction)->receiveAck((SipRequestAck *) request);

	else
		((TransactionServerNonInvite *) transaction)->receiveRequest(request);

	return true;
}

bool UserAgentServer::receiveMsg(SipRequest *request, Transaction *transaction)
{
	SipMessageType requestType = request->getMsgType();

	if ((requestType != SIP_REQUEST_ACK)) //&& (requestType != SIP_REQUEST_CANCEL))
	{
		SipHeaderRequire *require = (SipHeaderRequire *) request->getHeader(SIP_HEADER_REQUIRE);
		if (require)
		{
			SipResponse *response = createResponse(420, request);
			SipHeaderUnsupported *unsupported = new SipHeaderUnsupported();
			response->addHeader(unsupported);

			for (int i = 0; i < require->getOptionSize(); i++)
			{
				const char *option = require->getOption(i);
				unsupported->addOption(option);
			}

			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}
	}

	if (!ua->dialog.isActive())
	{
		SipHeaderTo *headerTo = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);
		if (!headerTo)
			return false;

		const char *remoteTag = headerTo->getTag();
		if (remoteTag)
		{
			SipResponse *response = createResponse(481, request);
			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}
	}else
	{
		SipHeaderCSeq *headerCSeq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
		if (!headerCSeq)
			return false;

		unsigned int sequenceNumber = headerCSeq->getSequence();
		unsigned int dialogSequenceNumber = ua->dialog.remoteSequenceNumber;

		if ((dialogSequenceNumber == 0) || (sequenceNumber > dialogSequenceNumber))
			ua->dialog.remoteSequenceNumber = sequenceNumber;
		/*else if (sequenceNumber < dialogSequenceNumber) //TODO: Descomentar quando verificar Dialog!
		{
			SipResponse *response = createResponse(500, request);
			sendResponse(response, requestType, transaction);
			//delete response;
			return false;
		}*/
	}


	switch (requestType)
	{
		case SIP_REQUEST_ACK:		return true; //receiveAck((SipRequestAck *) request, transaction);
		case SIP_REQUEST_BYE:		return receiveBye((SipRequestBye *) request, transaction);
		case SIP_REQUEST_INVITE:	return receiveInvite((SipRequestInvite *) request, transaction);
		case SIP_REQUEST_MESSAGE:	return receiveMessage((SipRequestMessage *) request, transaction);
		case SIP_REQUEST_NOTIFY:	return true; //receiveNotify((SipRequestNotify *) request, transaction);
		case SIP_REQUEST_SUBSCRIBE:	return receiveSubscribe((SipRequestSubscribe *) request, transaction);
		default:					break;
	}

	return false;
}

bool UserAgentServer::receiveBye(SipRequestBye *request, Transaction *transaction)
{
	if (!ua->dialog.isActive())
	{
		SipResponse *response = createResponse(481, request);
		sendResponse(response, SIP_REQUEST_BYE, transaction);
		//delete response;
	}else
	{
		SipResponse *response = createResponse(200, request);
		sendResponse(response, SIP_REQUEST_BYE, transaction);
		//delete response;

		ua->dialog.clear();
		SipManager::callback(SIP_SESSION_TERMINATED, ua);
	}

	return true;
}

bool UserAgentServer::receiveInvite(SipRequestInvite *request, Transaction *transaction)
{
	if (!ua->dialog.isActive())
	{
		SipResponse *response = createResponse(200, request);

		int recordRouteNum = request->getNumHeader(SIP_HEADER_RECORD_ROUTE);
		for (int i = 0; i < recordRouteNum; i++)
		{
			SipHeaderRecordRoute *headerRecordRoute = (SipHeaderRecordRoute *) request->getHeader(SIP_HEADER_RECORD_ROUTE, i);
			response->addHeader(new SipHeaderRecordRoute(*headerRecordRoute));
		}

		SipHeaderAllow *headerAllow = new SipHeaderAllow();
		for (int i = 0; i < (SIP_MESSAGE_TYPE_INVALID - 1); i++)
			headerAllow->addAllowed((SipMessageType) i);
		response->addHeader(headerAllow);

		createDialog(request, response);
		sendResponse(response, SIP_REQUEST_INVITE, transaction);
		//delete response;

		//TODO: 13.3.1.4 The INVITE is Accepted: Reenviar periodicamente a resposta até receber ACK
	}else
	{
		SipHeaderContact *contact = (SipHeaderContact *) request->getHeader(SIP_HEADER_CONTACT);
		if (contact)
		{
			const char *target = contact->getAddress();
			if ((target) && (!strcmp(ua->dialog.remoteTarget, target)))
				ua->dialog.setRemoteTarget(target);
		}
	}

	return true;
}

bool UserAgentServer::receiveMessage(SipRequestMessage *request, Transaction *transaction)
{
	SipResponse *response = createResponse(200, request);
	sendResponse(response, SIP_REQUEST_MESSAGE, transaction);
	//delete response;

	SipBody *body = request->getBody();
	if ((!body) || (body->getBodyType() != SIP_BODY_TEXT_PLAIN))
		return false;

	ua->textReceived = ((SipTextPlainBody *) body)->getText();
	SipManager::callback(SIP_MESSAGE_RECEIVED, ua);
	ua->textReceived = 0;
	return true;
}

bool UserAgentServer::receiveSubscribe(SipRequestSubscribe *request, Transaction *transaction)
{
	SipHeaderEvent *headerEvent = (SipHeaderEvent *) request->getHeader(SIP_HEADER_EVENT);
	if (!headerEvent)
		return false;

	SipEventPackage eventType = headerEvent->getType();

	if (((!ua->subscription.isActive()) && (eventType == SIP_EVENT_PRESENCE)) ||
		((ua->subscription.isActive()) && (eventType == ua->subscription.eventType)))
	{
		SipResponse *response = createResponse(200, request);

		SipHeaderExpires *expires = (SipHeaderExpires *) request->getHeader(SIP_HEADER_EXPIRES);
		SipHeaderExpires *headerExpires = new SipHeaderExpires();
		unsigned int time = (expires) ? expires->getNumber() : 3600;
		headerExpires->setNumber(time);
		response->addHeader(headerExpires);

		if ((!ua->dialog.isActive()) && (time > 0))
			createDialog(request, response);

		sendResponse(response, SIP_REQUEST_SUBSCRIBE, transaction);
		//delete response;

		if (ua->dialog.isActive())
		{
			if (time > 0)
				; //TODO: Acionar ou atualizar timer por tempo do Expires
			else
			{
				ua->subscription.clear();
				ua->dialog.clear(); //TODO: Deletar mesmo se tem INVITE session?
				SipManager::callback(SIP_SUBSCRIPTION_TERMINATED, ua);
			}
		}
	}else
	{
		SipResponse *response = createResponse(489, request);
		sendResponse(response, SIP_REQUEST_SUBSCRIBE, transaction);
		//delete response;
	}

	return true;
}

bool UserAgentServer::createDialog(SipRequest *request, SipResponse *response)
{
	SipHeaderFrom *from = (SipHeaderFrom *) request->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *callId = (SipHeaderCallID *) request->getHeader(SIP_HEADER_CALLID);
	SipHeaderCSeq *cseq = (SipHeaderCSeq *) request->getHeader(SIP_HEADER_CSEQ);
	SipHeaderContact *contact = (SipHeaderContact *) request->getHeader(SIP_HEADER_CONTACT);
	SipHeaderTo *toRequest = (SipHeaderTo *) request->getHeader(SIP_HEADER_TO);
	SipHeaderTo *toResponse = (SipHeaderTo *) response->getHeader(SIP_HEADER_TO);

	if ((!from) || (!callId) || (!cseq) || (!toRequest) || (!toResponse) || (!contact))
		return false;

	const char *id = callId->getString();
	const char *localTag = toResponse->getTag();
	const char *remoteTag = from->getTag(); //Pode ser NULL, compatibilidade com RFC 2543
	const char *localURI = toRequest->getAddress();
	const char *remoteURI = from->getAddress();
	const char *target = contact->getAddress();
	unsigned int sequenceNumber = cseq->getSequence();

	if ((!id) || (!localTag) || (!localURI) || (!remoteURI) || (!target))
		return false;

	if (request->getMsgType() == SIP_REQUEST_SUBSCRIBE)
	{
		SipHeaderEvent *event = (SipHeaderEvent *) request->getHeader(SIP_HEADER_EVENT);
		if (!event)
			return false;

		SipEventPackage eventType = event->getType();
		const char *eventId = event->getId();

		if (eventType == SIP_EVENT_PACKAGE_INVALID)
			return false;

		ua->subscription.setSubscription(eventType, eventId);
	}

	ua->dialog.setDialog(id, localTag, remoteTag, 0, sequenceNumber, localURI, remoteURI, target);

	int recordRouteNum = response->getNumHeader(SIP_HEADER_RECORD_ROUTE);
	for (int i = 0; i < recordRouteNum; i++)
	{
		SipHeaderRoute *route = (SipHeaderRoute *) response->getHeader(SIP_HEADER_RECORD_ROUTE, i);
		ua->dialog.addRouteBack(route);
	}

	if (request->getMsgType() == SIP_REQUEST_SUBSCRIBE)
		SipManager::callback(SIP_SUBSCRIPTION_INITIATED, ua);
	else
		SipManager::callback(SIP_SESSION_INITIATED, ua);
	return true;
}

//-------------------------------------------

UserAgent::UserAgent(const char *uri) : uac(this), uas(this), link(this)
{
	this->uri = createString(uri);
	this->textReceived = 0;

	for (int i = 0; i < SIP_TIMER_COUNT; i++)
	{
		timerValues[i] = 0;
		timerHandlers[i] = 0;
		timerAlarms[i] = 0;
	}
}

UserAgent::~UserAgent()
{
	Simple_List<Transaction>::Iterator it = transactions.begin();
	while (it != transactions.end())
	{
		Transaction *transaction = it->object();
		transactions.remove(it++);
		delete transaction;
	}

	for (int i = 0; i < SIP_TIMER_COUNT; i++)
	{
		if (timerHandlers[i])
			delete timerHandlers[i];
		if (timerAlarms[i])
			delete timerAlarms[i];
	}
}

/*bool UserAgent::matchingDialog(SipMessage *msg)
{
	if ((!dialog.isActive()) || (!dialog.callID) || (!dialog.localTag)) //|| (!dialog.remoteTag))
		return false;

	SipHeaderFrom *headerFrom = (SipHeaderFrom *) msg->getHeader(SIP_HEADER_FROM);
	SipHeaderCallID *headerCallId = (SipHeaderCallID *) msg->getHeader(SIP_HEADER_CALLID);
	SipHeaderTo *headerTo = (SipHeaderTo *) msg->getHeader(SIP_HEADER_TO);

	if ((!headerFrom) || (!headerCallId) || (!headerTo))
		return false;

	const char *callId = headerCallId->getString();
	const char *localTag = 0; //No INVITE não possui ainda!
	const char *remoteTag = 0; //Pode ser NULL, compatibilidade com RFC 2543

	if (msg->getMsgType() == SIP_RESPONSE)
	{
		localTag = headerFrom->getTag();
		remoteTag = headerTo->getTag();
	}else
	{
		localTag = headerTo->getTag();
		remoteTag = headerFrom->getTag();
	}

	if ((!callId)) //|| (!localTag) || (!remoteTag))
		return false;

	bool ret1 = !strcmp(dialog.callID, callId);
	bool ret2 = (localTag) ? !strcmp(dialog.localTag, localTag) : true;
	bool ret3 = (remoteTag) ? !strcmp(dialog.remoteTag, remoteTag) : true;
	if ((!ret1) || (!ret2) || (!ret3))
		return false;

	return true;
}*/

Transaction *UserAgent::matchingTransaction(SipMessage *msg)
{
	Simple_List<Transaction>::Iterator it = transactions.begin();
	while (it != transactions.end())
	{
		Transaction *transaction = it->object();
		it++;

		if (transaction->matchingTransaction(msg))
			return transaction;
	}
	return 0;
}

void UserAgent::startTimer(SipTimer timer, Transaction *p)
{
	switch (timer)
	{
		case SIP_TIMER_A: timerHandlers[SIP_TIMER_A] = new Functor_Handler<Transaction>(&TransactionClientInvite::timerACallback, p);		break;
		case SIP_TIMER_B: timerHandlers[SIP_TIMER_B] = new Functor_Handler<Transaction>(&TransactionClientInvite::timerBCallback, p);		break;
		//case SIP_TIMER_C: timerHandlers[SIP_TIMER_C] = new Functor_Handler<Transaction>(&timerCCallback);									break;
		case SIP_TIMER_D: timerHandlers[SIP_TIMER_D] = new Functor_Handler<Transaction>(&TransactionClientInvite::timerDCallback, p);		break;
		case SIP_TIMER_E: timerHandlers[SIP_TIMER_E] = new Functor_Handler<Transaction>(&TransactionClientNonInvite::timerECallback, p);	break;
		case SIP_TIMER_F: timerHandlers[SIP_TIMER_F] = new Functor_Handler<Transaction>(&TransactionClientNonInvite::timerFCallback, p);	break;
		case SIP_TIMER_G: timerHandlers[SIP_TIMER_G] = new Functor_Handler<Transaction>(&TransactionServerInvite::timerGCallback, p);		break;
		case SIP_TIMER_H: timerHandlers[SIP_TIMER_H] = new Functor_Handler<Transaction>(&TransactionServerInvite::timerHCallback, p);		break;
		case SIP_TIMER_I: timerHandlers[SIP_TIMER_I] = new Functor_Handler<Transaction>(&TransactionServerInvite::timerICallback, p);		break;
		case SIP_TIMER_J: timerHandlers[SIP_TIMER_J] = new Functor_Handler<Transaction>(&TransactionServerNonInvite::timerJCallback, p);	break;
		case SIP_TIMER_K: timerHandlers[SIP_TIMER_K] = new Functor_Handler<Transaction>(&TransactionClientNonInvite::timerKCallback, p);	break;
		default: return;
	}

	timerAlarms[timer] = new Alarm(timerValues[timer], timerHandlers[timer], 1);
}

void UserAgent::stopTimer(SipTimer timer)
{
	if (timerHandlers[timer])
	{
		delete timerHandlers[timer];
		timerHandlers[timer] = 0;
	}

	if (timerAlarms[timer])
	{
		delete timerAlarms[timer];
		timerAlarms[timer] = 0;
	}
}

//-------------------------------------------

void SendRTP::sendData(const char *destination, unsigned short port, char *data, unsigned int size)
{
	data[3] = sequence;
	data[2] = sequence >> 8;

	data[7] = timestamp;
	data[6] = timestamp >> 8;
	data[5] = timestamp >> 16;
	data[4] = timestamp >> 24;

	//db<SendRTP>(INF) << "SendRTP::sendData -> Sending data to " << destination << ":" << port << " (size: " <<
	//					size << ", seq: " << sequence << ", timestamp: " << timestamp << ")\n";

	UDP::Address dst(IP::Address(destination), port);
	socket.set_remote(dst);

	if (socket.send(data, size) <= 0)
	{
		db<SendRTP>(WRN) << "SendRTP::sendData -> Failed to send data\n";
		//return;
	}

	sequence++;
	timestamp += 0xA0;
}

__END_SYS
