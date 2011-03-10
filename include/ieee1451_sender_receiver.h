#ifndef __ieee1451_sender_receiver_h
#define __ieee1451_sender_receiver_h

#include <ip/ip.h>
#include <ip/udp.h>

__BEGIN_SYS

#define IEEE1451_PORT 55667

typedef int (*PACKET_CALLBACK)(unsigned char type, unsigned short transId, const IP::Address &source, const char *message, unsigned int length);

struct Packet
{
	unsigned char type;
	unsigned short transId;
	unsigned int len;
};

enum PacketType
{
	CONNECT = 1,
	DISCONNECT = 2,
	MESSAGE = 3
};


class SenderReceiver
{
private:
	UDP udp;

	static PACKET_CALLBACK callback;

public:
	SenderReceiver() : udp(IP::instance()), socket(&udp) {};
	~SenderReceiver() {};

	void send(unsigned char type, unsigned short transId, const IP::Address &dst, const char *message, unsigned int length);
	static void registerPacketHandler(PACKET_CALLBACK callback) { SenderReceiver::callback = callback; };

private:
	class MySocket : public UDP::Socket
	{
	public:
		MySocket(UDP *udp) : UDP::Socket(udp, UDP::Address(Traits<IP>::ADDRESS, IEEE1451_PORT), UDP::Address(Traits<IP>::BROADCAST, IEEE1451_PORT)) {};
		virtual ~MySocket() {};

		void received(const UDP::Address &src, const char *data, unsigned int size);
	} socket;
};

__END_SYS

#endif
