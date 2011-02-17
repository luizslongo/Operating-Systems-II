#include <ip/tcp.h>

//#include <cpu.h>

__BEGIN_SYS

TCP::TCP(IP * ip) : _ip(ip)
{
	_ip->attach(this, ID_TCP);
}

TCP::~TCP()
{
	_ip->detach(this, ID_TCP);
}

// Called by IP's notify(...)

void TCP::update(Data_Observed<IP::Address> *ob, long c, IP::Address src,
	         IP::Address dst, void *data, unsigned int size)
{
	Header& hdr = *reinterpret_cast<Header*>(data);

	db<IP>(TRC) << "TCP::update: "<< hdr << endl;

	if (!(hdr.validate_checksum(src,dst,size - hdr.size()))) {
		db<TCP>(INF) << "TCP checksum failed for incomming packet!\n";
		return;
	}
	
	int len = static_cast<int>(size) - static_cast<int>(hdr.size());
	if (len < 0) {
		db<TCP>(INF) << "Misformed TCP segment received\n";
		return;
	}
	
	notify(TCP::Address(src,hdr.src_port()),
	       TCP::Address(dst,hdr.dst_port()),
	       (int) hdr.dst_port(), data, size);
}


// Called by TCP's notify(...)

void TCP::Socket::update(Data_Observed<TCP::Address> *o, long c, TCP::Address src,
                         TCP::Address dst, void *data, unsigned int size)
{
	Header& hdr = *reinterpret_cast<Header*>(data);
	int len = static_cast<int>(size) - static_cast<int>(hdr.size());
	
	if (!((_remote.port() == src.port()) || _remote.port() == 0))
	{
		db<TCP>(TRC) << "TCP Segment does not belong to us\n";		
		return;
	}
		
	
	if (in_state(LISTEN)) _remote = src;
	(this->*state_handler)(hdr,&((char*)data)[hdr.size()],len);
}

// Header stuff

TCP::Header::Header(u32 seq,u32 ack)
{
	memset(this,0,sizeof(this));
	seq_num(seq);
	ack_num(ack);
}

u16 TCP::Header::_checksum(IP::Address &src,IP::Address &dst,u16 len)
{
	len += size();

	Pseudo_Header phdr;
	phdr.src_ip = (u32)src;
	phdr.dst_ip = (u32)dst;
	phdr.zero = 0;
	phdr.protocol = ID_TCP;
	phdr.length = CPU::htons(len);

	unsigned int sum = 0;

	u8 * ptr = reinterpret_cast<u8 *>(this);
	unsigned int i;

	for(i = 0; i < len-1; i+=2)
		sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];
	if(len & 1) {
		sum += ptr[len-1];
	}

	ptr = reinterpret_cast<u8 *>(&phdr);
	for(i = 0;i < sizeof(Pseudo_Header); i+=2)
		sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

void TCP::Header::_checksum(IP::Address &src,IP::Address &dst,SegmentedBuffer * sb)
{
	u16 len;
	len = size();

	Pseudo_Header phdr;
	phdr.src_ip = (u32)src;
	phdr.dst_ip = (u32)dst;
	phdr.zero = 0;
	phdr.protocol = ID_TCP;
	phdr.length = CPU::htons(len);

	unsigned int sum = 0;

	const u8 * ptr = reinterpret_cast<u8 *>(this);
	unsigned int i;

	do {
		for(i = 0; i < len-1; i+=2)
			sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];
		if(len & 1) {
			sum += ptr[len-1];
		}
		if (sb) {		
			ptr = reinterpret_cast<const u8 *>(sb->data());
			len = sb->size();
			sb  = sb->next();
		} else {
			break;
		}
	} while (1);

	ptr = reinterpret_cast<u8 *>(&phdr);
	for(i = 0;i < sizeof(Pseudo_Header); i+=2)
		sum += (((u16)(ptr[i+1]) & 0x00FF) << 8) | ptr[i];

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	_chksum = ~sum;
}

void TCP::Header::checksum(IP::Address &src,IP::Address &dst,u16 len)
{
	_chksum = _checksum(src,dst,len);
}

bool TCP::Header::validate_checksum(IP::Address &src,IP::Address &dst,u16 len)
{
	u16 tmp = _checksum(src,dst,len);
	return tmp == 0x0000;
}

// Socket stuff

TCP::Socket::Socket(TCP * tcp,const Address &remote,const Address &local)
	: _tcp(tcp), _remote(remote), _local(local), _rtt(5000000)
{
	CHANGE_STATE(CLOSED);
	_tcp->attach(this, _local.port());
}

TCP::Socket::~Socket()
{
	_tcp->detach(this, _local.port());
}

s32 TCP::Socket::send(Header * hdr, SegmentedBuffer * sb)
{
	// fill header
	hdr->src_port(_local.port());
	hdr->dst_port(_remote.port());
	hdr->_hdr_off = 5; // our header is always 20 bytes
	hdr->wnd(rcv_wnd);
	hdr->_checksum(_local,_remote,sb);

	// hdr + sb
	SegmentedBuffer nsb(hdr,hdr->size());
	nsb.append(sb);
	return _tcp->ip()->send(_local,_remote,&nsb,TCP::ID_TCP) - hdr->size();
}


TCP::ClientSocket::ClientSocket(TCP * tcp,const Address& remote,const Address& local) 
	: Socket(tcp,local,remote)
{
//	enter();
//	mac_update();

	CHANGE_STATE(SYN_SENT);
	snd_ini = Pseudo_Random::random() & 0x00FFFFFF;
	snd_una = snd_ini;
	snd_nxt = snd_ini + 1;
	rcv_wnd = 512;

	Header hdr(snd_ini, 0);
	hdr._syn = true;
	send(&hdr,0);
//	wait();

//	if (in_state(ESTABLISHED)) {
//		send_ack();
//		leave();
//		return true;
//	}
//	db<TCP>(ERR) << "TCP::connect() error: "<<_error<<endl;
//	leave();
//	return false;
}

TCP::ServerSocket::ServerSocket(TCP * tcp,const Address& local) 
	: Socket(tcp,local,Address(0,0))
{
	CHANGE_STATE(LISTEN);
	rcv_wnd = 0;
}

void TCP::Socket::__LISTEN(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

	if (r._syn && !r._rst && !r._fin) {
		rcv_nxt = r.seq_num()+1;
		rcv_ini = r.seq_num();
		snd_wnd = r.wnd();

		CHANGE_STATE(SYN_RCVD);
		
		snd_ini = Pseudo_Random::random() & 0x0000FFFF;
		snd_nxt = snd_ini+1;
		snd_una = snd_ini;
		
		Header s(snd_ini,rcv_nxt);
		s._syn = true;
		s._ack = true;
		send(&s,0);
			
	} else {
		_remote = Address((u32)0,(u16)0);
	}
}

/**
	Este método é executado apenas em conexões iniciadas
	com TCP::connect() e seu objetivo é tratar o par SYN+ACK
	de resposta ao nosso SYN enviado anteriormente.
*/
void TCP::Socket::__SYN_SENT(const Header& r,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

	if (r._rst || r._fin) {
			error(ERR_REFUSED);
			CHANGE_STATE(CLOSED);
			closed();
	}
	else if (r._ack) {
		if ((r.ack_num() <= snd_ini) || (r.ack_num() > snd_nxt)) {
			error(ERR_RESET);
			CHANGE_STATE(CLOSED);
			closed();
		} else if ((r.ack_num() >= snd_una) && (r.ack_num() <= snd_nxt)) {
			if (r._syn) {
				rcv_nxt = r.seq_num() + 1;
				rcv_ini = r.seq_num();
				snd_una = r.ack_num();
				snd_wnd = r.wnd();
				if (snd_una > snd_ini) {
					CHANGE_STATE(SYN_RCVD);
				} else {
					CHANGE_STATE(ESTABLISHED);
					send_ack();
					connected();
				}
			} else {
				// TODO: discover what to do here
			}
		}
	} else if (!r._rst && r._syn) {
		rcv_nxt = r.seq_num() + 1;
		snd_ini = r.seq_num();
		snd_wnd = r.wnd();
		CHANGE_STATE(SYN_RCVD);
	}

}
/**
	Este estado geralmente só é alcançado após o LISTEN,
	já que o handshake de 3-vias é mais comum do que o de 4.

	Ao receber um ACK a conexão é estabelecida.
	Neste ponto a atualização da janela do host remoto (snd_wnd)
	é importante.
*/
void TCP::Socket::__SYN_RCVD(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	
	if (!check_seq(r,len)) 
		return;
		
	if (r._rst || r._fin) {
		error(ERR_RESET);
		CHANGE_STATE(CLOSED);
		closed();
	}
	else if (r._ack) {
		snd_wnd = r.wnd();
		CHANGE_STATE(ESTABLISHED);
		connected();
	}
}

void TCP::Socket::__RCVING(const Header &r,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

	rcv_nxt += len;
	received(data,len);
	//TODO: ack here 
}

void TCP::Socket::__SNDING(const Header &r,const char* data, u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	
	if (r._ack) {
		//signal();
		//sent(???);
	}
}

void TCP::Socket::__ESTABLISHED(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	
	if (!check_seq(r,len))
		return;
		
	if (r._rst) {
		error(ERR_RESET);
		CHANGE_STATE(CLOSED);
		closed();
	}
	else if (r.seq_num() == rcv_nxt) { // implicit reject out-of-order segments
		snd_wnd = r.wnd();

		if (len)
			__RCVING(r,data,len);

		else // TODO: this is wrong
			__SNDING(r,data,len);

		if (r._fin) CHANGE_STATE(CLOSE_WAIT);
	}
	else {
		db<TCP>(TRC) << "out of order segment received\n";
	}
}


void TCP::Socket::__FIN_WAIT1(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	
	if (!check_seq(r,len))
		return;

	if (r._ack && !r._fin) { // TODO: check snd_una
		CHANGE_STATE(FIN_WAIT2);
		//rcv_nxt = r.seq_num() + len;
		//signal();
	}
	if (r._ack && r._fin) {
		CHANGE_STATE(TIME_WAIT);
		//signal();
	}
	if (!r._ack && r._fin) {
		CHANGE_STATE(CLOSING);
		//signal();
	}
}
void TCP::Socket::__FIN_WAIT2(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	//signal();
}
void TCP::Socket::__CLOSE_WAIT(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	if (!check_seq(r,len))
		return;
		
	//if (_transfer == SNDING)
	//__SNDING(r,data,len);
}
void TCP::Socket::__CLOSING(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	if (!check_seq(r,len))
		return;
	//signal();
}
void TCP::Socket::__LAST_ACK(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	if (!check_seq(r,len))
		return;
	if (r._ack) {
		CHANGE_STATE(CLOSED);
		closed();
		//signal();
	}
}

void TCP::Socket::__TIME_WAIT(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
	if (!check_seq(r,len))
		return;
	if (r._fin && r._ack) {
		CHANGE_STATE(CLOSED);
		snd_nxt++; // ?
		closed();
	}
}

void TCP::Socket::__CLOSED(const Header&,const char*,u16)
{
	// does nothing
}

void TCP::Socket::send_ack() {
	Header s(snd_nxt,rcv_nxt);
	s._ack = true;
	send(&s,0);
}

void TCP::Socket::send_fin() {
	Header s(snd_nxt,rcv_nxt);
	s._fin = true;
	s._ack = true;
	send(&s,0);
}

bool TCP::Socket::check_seq(const Header &h,u16 len) {
	if ((len <= rcv_wnd) &&
		(h.seq_num() == rcv_nxt)) {
		db<TCP>(TRC) << "TCP: check_seq() == true\n";
		return true;
	}
	db<TCP>(TRC) << "TCP: check_seq() == false\n";
	return false;
}

__END_SYS

