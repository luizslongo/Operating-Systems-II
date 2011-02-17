#include <ip/icmp.h>

__BEGIN_SYS

ICMP::ICMP(IP* ip) : _ip(ip)
{
	_ip->attach(this, ICMP_ID);
}

ICMP::~ICMP()
{
	_ip->detach(this, ICMP_ID);
}

void ICMP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
		       IP::Address dst, void *data, unsigned int size)
{
	Packet& packet = *reinterpret_cast<Packet*>(data);
	if (IP::calculate_checksum(data,size) != 0) {
		db<IP>(TRC) << "ICMP checksum error\n";
		return;
	}
	
	if (Traits<IP>::echo_reply && (packet.type() == ECHO)) { // PONG
		Packet reply(ECHO_REPLY,0,packet.id(),packet.sequence(),packet._data);
		send(dst,src,reply);
	}
	
	if (packet.type() == ECHO_REPLY) {
		db<IP>(INF) << "Echo reply from " << src << endl;
	}
	
	notify(src,dst,c,data,size);
}

ICMP::Packet::Packet(Type type,Code code, unsigned short id,unsigned short seq,
		       const char * data,short size)
	: _type(type),
	_code(code),
	_checksum(0),
	_id(CPU::htons(id)),
	_sequence(CPU::htons(seq))
{
	memset(_data, 0, 56);
	if (data) memcpy(_data,data,size < 56 ? size : 56);
}

void ICMP::send(IP::Address from,IP::Address to,Packet& pkt)
{
	// Thou shall not calculate the checksum in ctor body!
	pkt._checksum = 0;
	pkt._checksum = IP::calculate_checksum(&pkt, sizeof(pkt));
	SegmentedBuffer sb(pkt.raw(),sizeof(pkt));
	_ip->send(from,to,&sb,ICMP_ID);
}

__END_SYS
