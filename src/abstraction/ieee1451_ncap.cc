#include <ieee1451_ncap.h>
#include <ieee1451_ncap_application.h>

__BEGIN_SYS


//-------------------------------------------

TEDS_NCAP::TEDS_NCAP(char id, const char *teds, unsigned int length, bool subblock)
	: id(id), link(this)
{
	unsigned int begin, end;

	if (subblock)
	{
		begin = 0;
		end = length;
	}else
	{
		begin = 4;
		end = length - 2;
	}

	for (unsigned int i = begin; i < end; )
	{
		unsigned short type = teds[i++] & 0xff;
		unsigned short len = teds[i++] & 0xff;

		char *value = new char[len];
		for (unsigned int j = 0; j < len; j++, i++)
			value[j] = teds[i];

		TLV *tlv = new TLV(type, len, value);
		tlvs.insert(&tlv->link);
	}
}

TEDS_NCAP::~TEDS_NCAP()
{
	Simple_List<TLV>::Iterator it = tlvs.begin();
	while (it != tlvs.end())
	{
		Simple_List<TLV>::Element *el = it++;
		TLV *tlv = el->object();
		tlvs.remove(&tlv->link);
		delete tlv;
	}
}

TLV *TEDS_NCAP::getTLV(char type)
{
	Simple_List<TLV>::Iterator it = tlvs.begin();
	while (it != tlvs.end())
	{
		TLV *tlv = it->object();
		it++;

		if (tlv->type == type)
			return tlv;
	}

	return 0;
}

//-------------------------------------------

//const unsigned short IEEE1451TIMChannel::TIM_CHANNEL_NUMBER = 0;

IEEE1451Channel::~IEEE1451Channel()
{
	Simple_List<TEDS_NCAP>::Iterator it = tedss.begin();
	while (it != tedss.end())
	{
		Simple_List<TEDS_NCAP>::Element *el = it++;
		TEDS_NCAP *teds = el->object();
		tedss.remove(&teds->link);
		delete teds;
	}
}

TEDS_NCAP *IEEE1451Channel::getTEDS(char id)
{
	Simple_List<TEDS_NCAP>::Iterator it = tedss.begin();
	while (it != tedss.end())
	{
		TEDS_NCAP *teds = it->object();
		it++;

		if (teds->id == id)
			return teds;
	}

	return 0;
}

//-------------------------------------------

IEEE1451dot0_NCAP *IEEE1451dot0_NCAP::dot0 = 0;

IEEE1451dot0_NCAP::IEEE1451dot0_NCAP()
{
	application = 0;
	IEEE1451dot5_NCAP::getInstance();
}

IEEE1451dot0_NCAP *IEEE1451dot0_NCAP::getInstance()
{
	if (!dot0)
		dot0 = new IEEE1451dot0_NCAP();
	return dot0;
}

char *IEEE1451dot0_NCAP::createCommand(unsigned short channelNumber, unsigned short command, const char *args, unsigned int length)
{
	unsigned int size = sizeof(Command) + length;
	char *buffer = new char[size];

	Command *cmd = (Command *) buffer;
	cmd->channelNumber = channelNumber;
	cmd->command = command;
	cmd->length = length;

	for (unsigned short i = 0; i < length; i++)
		buffer[i + sizeof(Command)] = args[i];

	return buffer;
}

unsigned short IEEE1451dot0_NCAP::sendCommand(const IP::Address &destination, const char *message, unsigned int length)
{
	static unsigned int idGenerator = 1;
	unsigned short transId = idGenerator++;

	IEEE1451dot5_NCAP::getInstance()->sendMessage(transId, destination, message, length);
	return transId;
}

void IEEE1451dot0_NCAP::timConnected(const IP::Address &address)
{
	application->reportTimConnected(address);
}

void IEEE1451dot0_NCAP::timDisconnected(const IP::Address &address)
{
	application->reportTimDisconnected(address);
}

void IEEE1451dot0_NCAP::receiveMessage(const IP::Address &address, unsigned short transId, const char *message, unsigned int length)
{
	if (transId == 0)
		application->reportTimInitiatedMessage(address, message, length);
	else
		application->reportCommandReply(address, transId, message, length);
}

void IEEE1451dot0_NCAP::errorOnSend(int errorCode, unsigned short transId)
{
	//Dependendo do erro, avisar que desconectou também!
	application->reportError(transId, errorCode);
}

//-------------------------------------------

IEEE1451dot5_NCAP *IEEE1451dot5_NCAP::dot5 = 0;

IEEE1451dot5_NCAP::IEEE1451dot5_NCAP()
{
	senderReceiver = new SenderReceiver();
	SenderReceiver::registerPacketHandler(messageCallback);
}

IEEE1451dot5_NCAP *IEEE1451dot5_NCAP::getInstance()
{
	if (!dot5)
		dot5 = new IEEE1451dot5_NCAP();
	return dot5;
}

void IEEE1451dot5_NCAP::sendMessage(unsigned short transId, const IP::Address &destination, const char *message, unsigned int length)
{
	db<IEEE1451dot5_NCAP>(INF) << "Enviando mensagem (transId=" << transId << ")...\n";
	senderReceiver->send(MESSAGE, transId, destination, message, length);
}

int IEEE1451dot5_NCAP::messageCallback(unsigned char type, unsigned short transId, const IP::Address &source, const char *message, unsigned int length)
{
	switch (type)
	{
		case CONNECT:
			IEEE1451dot0_NCAP::getInstance()->timConnected(source);
			break;

		case DISCONNECT:
			IEEE1451dot0_NCAP::getInstance()->timDisconnected(source);
			break;

		case MESSAGE:
			if (length > 0)
				IEEE1451dot0_NCAP::getInstance()->receiveMessage(source, transId, message, length);
			break;

		default:
			break;
	}

	return 0;
}

__END_SYS
