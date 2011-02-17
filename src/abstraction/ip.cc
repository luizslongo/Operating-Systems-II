#include <ip/ip.h>

__BEGIN_SYS

const IP::Address IP::NULL = IP::Address((u32)0);

u16 IP::Header::pktid = 0; // incremental packet id

// IP::Header
void IP::Header::calculate_checksum() {
    _checksum = 0;
    u16 * header = reinterpret_cast<u16 *>(this);
    u32 sum = 0;

    for(unsigned int i = 0; i < _ihl * sizeof(u16); i++)
    	sum += header[i];

    while(sum >> 16)
    	sum = (sum & 0xffff) + (sum >> 16);

    _checksum = ~sum;
}


// IP

IP::IP(unsigned int unit) :
	_nic(),
	_self(IP::NULL),
	_broadcast(255,255,255,255),
	_thread(0)
{
	_arpt.update(_broadcast, NIC::BROADCAST);

	if (Traits<IP>::dynamic == false) {
		_self = Address(Traits<IP>::ADDRESS);
		_arpt.update(_self,_nic.address());
		_broadcast = Address(Traits<IP>::BROADCAST);
		_arpt.update(_broadcast, NIC::BROADCAST);
		_netmask = Address(Traits<IP>::NETMASK);
	}


//	_nic.attach(this, NIC::ARP);
//	_nic.attach(this, NIC::RARP);
//	_nic.attach(this, NIC::IP);

	if (Traits<IP>::spawn_thread) {
		_thread = new Thread(IP::thread_main,this);
	}
}

IP::~IP() {
//	_nic.detach(this, NIC::ARP);
//	_nic.detach(this, NIC::RARP);
//	_nic.detach(this, NIC::IP);
	if (Traits<IP>::spawn_thread) {
		delete _thread;
	}
}

void IP::process_ip(char *data, u16 size)
{
    Header &pck_h = *reinterpret_cast<Header*>(data);
    if((u32)_self != (u32)0 && // We MUST accept anything if our IP address is not set
       (u32)(pck_h.dst_ip()) != (u32)(_self) &&
       (u32)(pck_h.dst_ip()) != (u32)(_broadcast))
    {
	db<IP>(INF) << "IP Packet discarded. dst= " << pck_h.dst_ip() << "\n";
	return;
    }
    else {
	    db<IP>(TRC) << "IP: " << pck_h << "\n" ;
    }

    if(pck_h.flags() != Header::MF_FLAG && pck_h.offset() == 0)
    {
	    notify(pck_h.src_ip(),pck_h.dst_ip(),(int)pck_h.protocol(),
		   &data[pck_h.hlength()], pck_h.length());
    }
    else {
	    db<IP>(WRN) << "Fragmented packet discarded" << endl;
	    // TODO: reasemble fragmented packets
    }
}


IP::MAC_Address IP::arp(const Address & la)
{
    for(unsigned int i = 0; i < Traits<Network>::ARP_TRIES; i++) {
	MAC_Address pa = _arpt.search(la);
	if(pa) {
	    db<IP>(TRC) << "IP::arp(la=" << la << ") => "
			     << pa << "\n";

	    return pa;
	}

	Condition * cond = _arpt.insert(la);
	_ARP::Packet request(_ARP::REQUEST, _nic.address(), address(),
			    NIC::BROADCAST, la);
	_nic.send(NIC::BROADCAST, NIC::ARP, &request, sizeof(_ARP::Packet));
	db<IP>(INF) << "IP::arp:request sent!\n";

	Condition_Handler handler(cond);
	//Alarm alarm(Traits<Network>::ARP_TIMEOUT, &handler, 1);
	Alarm alarm(100000, &handler, 1);
	cond->wait();
    }

    db<IP>(TRC) << "IP::arp(la=" << la << ") => not found!\n";

    return 0;
}

IP::Address IP::rarp(const MAC_Address & pa)
{
    for(unsigned int i = 0; i < Traits<Network>::ARP_TRIES; i++)
    {
		Address la(IP::NULL);

		Condition * cond = _arpt.insert(la);
		_ARP::Packet request(_ARP::RARP_REQUEST, pa, la, pa, la);
		_nic.send(NIC::BROADCAST, NIC::ARP, &request, sizeof(_ARP::Packet));
		db<IP>(INF) << "IP::rarp:request sent!\n";

		Condition_Handler handler(cond);
		//Alarm alarm(Traits<Network>::ARP_TIMEOUT, &handler, 1);
		Alarm alarm(100000, &handler, 1);
		cond->wait();

		if((u32)(la) != (u32)(IP::NULL)) {
			db<IP>(TRC) << "IP::rarp(pa=" << pa << ") => "
					 << la << "\n";

			return la;
		}
    }

    db<IP>(TRC) << "IP::rarp(pa=" << pa << ") => not found!\n";

    return IP::NULL;
}

/*void IP::update(NIC::Observed * o, int p)
{
	db<IP>(TRC) << "IP::update(o=" << o
		     << ",p=" << hex << p << dec << ")\n";

	//if (_thread) _thread->resume();
}*/

void IP::kill() {
	_alive = false;
	if (_thread) {
		_thread->resume();
	}
}

void IP::process_incoming() {
	db<IP>(TRC) << __PRETTY_FUNCTION__ << endl;
	NIC::Address src;
	NIC::Protocol prot;
	NIC::PDU data;
	int size = _nic.receive(&src, &prot, data, sizeof(NIC::PDU));
	if(size <= 0) {
		db<IP>(WRN) << "NIC::received error!" << endl;
		return;
	}
	if (prot == NIC::ARP) {
		_ARP::Packet packet = *reinterpret_cast<_ARP::Packet *>(data);
		db<IP>(INF) << "IP::update:ARP_Packet=" << packet << "\n";

		if((packet.op() == _ARP::REQUEST) && (packet.tpa() == address())) {
			_ARP::Packet reply(_ARP::REPLY, _nic.address(), address(),
					   packet.sha(), packet.spa());
			db<IP>(INF) << "IP::update: ARP_Packet=" << reply << "\n";
			_nic.send(packet.sha(), NIC::ARP, &reply, sizeof(_ARP::Packet));

			db<IP>(INF) << "IP::update: ARP request answered!\n";
		} else if((packet.op() == _ARP::REPLY)
			&& (packet.tha() == _nic.address())) {
			db<IP>(INF) << "IP::update: ARP reply received!\n";

			_arpt.update(packet.spa(), packet.sha());
		}
	}
	else if (prot == NIC::IP) {
		_arpt.update(reinterpret_cast<Header*>(data)->src_ip(), src);
		process_ip(data, size);
	}
	else
		db<IP>(TRC) << "IP::update:unknown packet type (" << prot << ")\n";
}

void IP::worker_loop() {
	_alive = true;
	_thread = Thread::self();
	while (_alive) {
		process_incoming();
		Thread::yield();
	}
}

s32 IP::send(const Address & from,const Address & to,SegmentedBuffer * data,Protocol proto) {
	Header hdr(from,to,proto,data->total_size());
	SegmentedBuffer pdu(&hdr,hdr.hlength(),data);

	int size = pdu.total_size();
	db<IP>(TRC) << "IP::send() " << size << " bytes" << endl;

	//TODO: put fragmentation here
	char sbuf[size];
	//TODO: possible stack overflow here, we must change NIC::send to accept SegmentedBuffers
	pdu.copy_to(sbuf,size);

	MAC_Address mac = NIC::BROADCAST;
	if (((u32)to & (u32)_netmask) == ((u32)_self & (u32)_netmask))
		mac = arp(to);
	else
		mac = arp(_gateway);

	if (_nic.send(mac,NIC::IP,sbuf,size) >= 0)
		return size;
	else
		return -1;

}

// From http://www.faqs.org/rfcs/rfc1071.html
u16 IP::calculate_checksum(void* ptr, u16 count)
{
	u32 sum = 0;

	unsigned char * _ptr = reinterpret_cast<unsigned char *>(ptr);
	u16 i;

	for(i = 0; i < count-1; i+=2)
		sum += (((unsigned short)(_ptr[i+1]) & 0x00FF) << 8) | _ptr[i];
	if(count & 1) {
		sum += _ptr[count-1];
	}

	while(sum >> 16)
		sum = (sum & 0xffff) + (sum >> 16);

	return ~sum;
}

int IP::thread_main(IP * thiz) {
	thiz->worker_loop();
	return 0;
}

__END_SYS

