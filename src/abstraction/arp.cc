#include <arp.h>

__BEGIN_SYS


ARP::ARP_Table ARP::_arp_tab;
#ifndef __avr8_h
ARP::HAddress  ARP::BCAST(0xff,0xff,0xff,0xff,0xff,0xff);
ARP::HAddress  ARP::NULL_ADDR(0,0,0,0,0,0);
#else
ARP::HAddress  ARP::BCAST(0);
ARP::HAddress  ARP::NULL_ADDR(0);
#endif

ARP::ARP(NIC &nic, LAddress me) : _nic(nic), 
				  _hmyself(_nic.address()), 
				  _lmyself(me) {
	NIC::attach(Ethernet_NIC::ARP, *this);
	NIC::attach(Ethernet_NIC::RARP, *this);
	_arp_tab.put(IP::BROADCAST, BCAST);
	db<ARP>(INF) << "ARP Object Created " << _hmyself << "\n";
}

void ARP::received(void *data, unsigned int size){
	ARP_Packet &pck = *reinterpret_cast<ARP_Packet*>(data);
	u16 opcode = pck.opcode();
	if(opcode == ARP_Packet::ARP_REQ &&
			_lmyself == pck.tla()){
		ARP_Packet rply(ARP_Packet::ARP_RPLY, _hmyself, _lmyself, pck.sha(), pck.sla());
		_nic.send(pck.sha(), Ethernet_NIC::ARP, &rply, sizeof(rply));
		db<ARP>(INF) << "ARP Request Received and Answered\n";
	}else if(opcode == ARP_Packet::ARP_RPLY &&
			_hmyself == pck.tha()){
		_arp_tab.put(pck.sla(), pck.sha());
		u32 logic = pck.tla();
		db<ARP>(INF) << "ARP Reply received: L=" << logic
				<< " H=" << pck.tha() << "\n";
	}
}

const ARP::HAddress& ARP::arp(ARP::LAddress laddr_dst){		
	HAddress *haddr = 0;
	unsigned int tries = 0;
	Alarm *a = 0;
	Handler_Condition *h = 0;
	Condition *c;
	do {
		if(_arp_tab.get(laddr_dst, haddr)){
			db<ARP>(INF) << "ARP Translation Found in local Table: "
					<< laddr_dst << " = " << *haddr << "\n";
			return *haddr;
		} else {
			ARP_Packet req(ARP_Packet::ARP_REQ, _hmyself,_lmyself,laddr_dst);
			_nic.send(BCAST, Ethernet_NIC::ARP,&req,sizeof(req));
			db<ARP>(INF) << "ARP Request Sent (" << (tries+1) << ") :" << req << "\n";
			
			if(!tries){
				c = _arp_tab.add(laddr_dst);
				h = new Handler_Condition(c);
				a = new Alarm(Traits<ARP>::TIMEOUT, h, Alarm::INFINITE);
			}
			c->wait();
		}
		tries++;
	} while(tries < Traits<ARP>::TRIES);

	if(a) delete a;
	if(h) delete h;
	db<ARP>(ERR) << "ARP Request Unanswered after " << tries << " attempts. Returning Null Address.\n";
	return NULL_ADDR;
}


Condition * ARP::ARP_Table::add(LAddress laddr){
	Entry *e = search(laddr);

	if(!e){
		e = new Entry(laddr, NULL_ADDR);
		_table.insert(&e->_link);
	}

	return &e->_c;
}

void ARP::ARP_Table::put(ARP::LAddress laddr, ARP::HAddress haddr){
	Entry *e = search(laddr);

	if(e){
		e->_haddr = haddr;
		e->_c.broadcast();
		return;
	}

	_table.insert(&(new Entry(laddr,haddr))->_link);
}

bool ARP::ARP_Table::get(ARP::LAddress laddr, ARP::HAddress* &haddr){
	Entry *e = search(laddr);

	if(e && e->_haddr != NULL_ADDR) {
		haddr = &e->_haddr;
		return true;
	}
	
	return false;
}

ARP::ARP_Table::Entry* ARP::ARP_Table::search(ARP::LAddress laddr){
	List::Element *e = _table.head();
	for(;e;e=e->next()){
		if(e->object()->_laddr == laddr){
			return e->object();	
		}
	}
	return 0;
}

Debug& operator<< (Debug &db,const ARP::ARP_Packet &a) {
	unsigned char *tha = (unsigned char*)(&a._tha);
	unsigned char *sha = (unsigned char*)(&a._sha);
	db << "{htype="  << CPU::ntohs(a._htype)
		<< ",ptype="  << (void*)CPU::ntohs(a._ptype)
		<< ",hlen="   << a._hlen
		<< ",plen="   << a._plen
		<< ",opcode=" << CPU::ntohs(a._opcode)
		<< ",sha="      << (void*)sha[0] << ":" << (void*)sha[1] << ":" 
				<< (void*)sha[2] << ":" << (void*)sha[3] << ":" 
				<< (void*)sha[4] << ":" << (void*)sha[5]
		<< ",sla="    << *(IP::Address*)(&a._sla)
		<< ",tha="    << (void*)tha[0] << ":" << (void*)tha[1] << ":" << (void*)tha[2] << ":" << (void*)tha[3] << ":" << (void*)tha[4] << ":" << (void*)tha[5]
		<< ",tla="    << *(IP::Address*)(&a._tla) 
		<< "}";
	return db;
}	

__END_SYS

