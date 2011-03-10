#include <ieee1451_sender_receiver.h>

__BEGIN_SYS


PACKET_CALLBACK SenderReceiver::callback = 0;

void SenderReceiver::send(unsigned char type, unsigned short transId, const IP::Address &dst, const char *message, unsigned int length)
{
	unsigned int dataLength = sizeof(Packet) + length;
	char *data = new char[dataLength];

	Packet *out = (Packet *) data;
	char *msg = data + sizeof(Packet);

	out->type = type;
	out->transId = transId;
	out->len = length;
	memcpy(msg, message, length);

	socket.set_remote(UDP::Address(dst, IEEE1451_PORT));

	int sent = 0;
	while ((sent = socket.send(data, dataLength)) <= 0)
	{
		db<SenderReceiver>(INF) << "Falha ao enviar mensagem.. Enviado(s) " << sent << " byte(s)!\n";
		Alarm::delay(240000); //TODO: 5 segundos
	}

	delete data;
}

void SenderReceiver::MySocket::received(const UDP::Address &src, const char *data, unsigned int size)
{
	Packet *in = (Packet *) data;
	const char *msg = data + sizeof(Packet);

	callback(in->type, in->transId, src.ip(), msg, in->len);
}

__END_SYS
