#include <ip/tcp.h>

//#include <cpu.h>

__BEGIN_SYS

// static data
TCP::Socket::Handler TCP::Socket::handlers[13] = { 
    &TCP::Socket::__LISTEN,     &TCP::Socket::__SYN_SENT,
    &TCP::Socket::__SYN_RCVD,   &TCP::Socket::__ESTABLISHED,
    &TCP::Socket::__FIN_WAIT1,  &TCP::Socket::__FIN_WAIT2,
    &TCP::Socket::__CLOSE_WAIT, &TCP::Socket::__CLOSING,
    &TCP::Socket::__LAST_ACK,   &TCP::Socket::__TIME_WAIT,
    &TCP::Socket::__CLOSED };

TCP::TCP(IP * ip) : _ip(ip)
{
    if (!ip)
        _ip = IP::instance();
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
	
	int len = size - hdr.size();
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
	int len = size - hdr.size();
	
	if (!((_remote.port() == src.port()) || _remote.port() == 0))
	{
		db<TCP>(TRC) << "TCP Segment does not belong to us\n";		
		return;
	}
		
	
	if (state() == LISTEN) _remote = src;
	(this->*state_handler)(hdr,&((char*)data)[hdr.size()],len);
}

// Header stuff

TCP::Header::Header(u32 seq,u32 ack)
{
	memset(this,0,sizeof(Header));
	seq_num(seq);
	ack_num(ack);
}

bool TCP::Header::validate_checksum(IP::Address &src,IP::Address &dst,u16 len)
{
    db<IP>(TRC) << __PRETTY_FUNCTION__ << endl;
    //db<IP>(TRC) << "len = " << len << endl;
    len += size();
   // db<IP>(TRC) << "len = " << len << endl;
    
    Pseudo_Header phdr((u32)src,(u32)dst,len);

    s32 sum = 0;

    sum = IP::calculate_checksum(this, len);
    //db<IP>(TRC) << "Partial sum = " << sum << endl;
    sum += IP::calculate_checksum(&phdr, sizeof(phdr));
   // db<IP>(TRC) << "Partial sum = " << sum << endl;
    while (sum >> 16)
        sum = (sum & 0xFFFF) + (sum >> 16);

    db<IP>(TRC) << "Validate checksum = " << sum << endl;
    return sum == 0xFFFF;
}

void TCP::Header::_checksum(IP::Address &src,IP::Address &dst,SegmentedBuffer * sb)
{
    db<IP>(TRC) << __PRETTY_FUNCTION__ << endl;
	u16 len;
	len = size();

    if (sb) len += sb->total_size();

	Pseudo_Header phdr((u32)src,(u32)dst,len);

    _chksum = 0;

	unsigned int sum = 0;

    sum = IP::calculate_checksum(&phdr, sizeof(phdr));
    sum += IP::calculate_checksum(this, size());

    while (sb) {
        sum += IP::calculate_checksum(sb->data(), sb->size());
        sb = sb->next();
    }
	_chksum = ~sum;
}

/*void TCP::Header::checksum(IP::Address &src,IP::Address &dst,u16 len)
{
    _chksum = 0;
	_chksum = _checksum(src,dst,len);
}*/

/*bool TCP::Header::validate_checksum(IP::Address &src,IP::Address &dst,u16 len)
{
	return _checksum(src,dst,len) == 0x0000;
}*/

// Socket stuff

TCP::Socket::Socket(TCP * tcp,const Address &remote,const Address &local)
	: _tcp(tcp), _remote(remote), _local(local), _rtt(5000000)
{
	state(CLOSED);
	_tcp->attach(this, _local.port());
}

TCP::Socket::~Socket()
{
	_tcp->detach(this, _local.port());
}

s32 TCP::Socket::_send(Header * hdr, SegmentedBuffer * sb)
{
	// fill header
	hdr->src_port(_local.port());
	hdr->dst_port(_remote.port());
	hdr->_hdr_off = 5; // our header is always 20 bytes
	hdr->wnd(rcv_wnd);
    hdr->chksum(0);
	hdr->_checksum(_local,_remote,sb);

	// hdr + sb
	SegmentedBuffer nsb(hdr,hdr->size());
	nsb.append(sb);
	return _tcp->ip()->send(_local,_remote,&nsb,TCP::ID_TCP) - hdr->size();
}

void TCP::Socket::send(const char *data,u16 len)
{
    Header hdr(snd_nxt,rcv_nxt-1);
    hdr._ack = true;
    snd_nxt += len;
    SegmentedBuffer sb(data,len);
    _send(&hdr,&sb);
}

void TCP::Socket::close()
{
    send_fin();
    state(CLOSE_WAIT);
}


TCP::ClientSocket::ClientSocket(TCP * tcp,const Address& remote,const Address& local) 
	: Socket(tcp,remote,local)
{
	state(SYN_SENT);
	snd_ini = Pseudo_Random::random() & 0x00FFFFFF;
	snd_una = snd_ini;
	snd_nxt = snd_ini + 1;
	rcv_wnd = 512;

	Header hdr(snd_ini, 0);
	hdr._syn = true;
	_send(&hdr,0);
}

TCP::ServerSocket::ServerSocket(TCP * tcp,const Address& local) 
	: Socket(tcp,Address(0,0),local)
{
	state(LISTEN);
	rcv_wnd = 1024;
}

void TCP::Socket::__LISTEN(const Header& r ,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;

	if (r._syn && !r._rst && !r._fin) {
		rcv_nxt = r.seq_num()+1;
		rcv_ini = r.seq_num();
		snd_wnd = r.wnd();

		state(SYN_RCVD);
		
		snd_ini = Pseudo_Random::random() & 0x0000FFFF;
				
		Header s(snd_ini,rcv_nxt);
		s._syn = true;
		s._ack = true;
		_send(&s,0);
        
        snd_nxt = snd_ini+1;
        snd_una = snd_ini;
			
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
			state(CLOSED);
			closed();
	}
	else if (r._ack) {
		if ((r.ack_num() <= snd_ini) || (r.ack_num() > snd_nxt)) {
			error(ERR_RESET);
			state(CLOSED);
			closed();
		} else if ((r.ack_num() >= snd_una) && (r.ack_num() <= snd_nxt)) {
			if (r._syn) {
				rcv_nxt = r.seq_num() + 1;
				rcv_ini = r.seq_num();
				snd_una = r.ack_num();
				snd_wnd = r.wnd();
				if (snd_una > snd_ini) {
					state(SYN_RCVD);
				} else {
					state(ESTABLISHED);
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
		state(SYN_RCVD);
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
		state(CLOSED);
		closed();
	}
	else if (r._ack) {
		snd_wnd = r.wnd();
		state(ESTABLISHED);
		connected();
	}
}

void TCP::Socket::__RCVING(const Header &r,const char* data,u16 len)
{
	db<TCP>(TRC) << __PRETTY_FUNCTION__ << endl;
    if (len) {
	    rcv_nxt += len - 1;
        send_ack();
        ++rcv_nxt;
	    received(data,len);
    } else {
        send_ack();
    }
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
		state(CLOSED);
		closed();
	}
	else if (r.seq_num() == rcv_nxt) { // implicit reject out-of-order segments
		snd_wnd = r.wnd();

		if (len)
			__RCVING(r,data,len);

		else // TODO: this is wrong
			__SNDING(r,data,len);

		if (r._fin) state(CLOSE_WAIT);
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
		state(FIN_WAIT2);
		//rcv_nxt = r.seq_num() + len;
		//signal();
	}
	if (r._ack && r._fin) {
		state(TIME_WAIT);
		//signal();
    }
	if (!r._ack && r._fin) {
		state(CLOSING);
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
	
    snd_nxt++;
    send_fin();
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
		state(CLOSED);
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
		state(CLOSED);
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
	_send(&s,0);
}

void TCP::Socket::send_fin() {
	Header s(snd_nxt,rcv_nxt);
	s._fin = true;
	s._ack = true;
	_send(&s,0);
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

