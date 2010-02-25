#include <ip.h>

__BEGIN_SYS

IP::Address IP::_self(CPU::htonl(Traits<IP>::ADDRESS));
const IP::Address 
IP::BROADCAST(CPU::htonl(Traits<IP>::BROADCAST ? Traits<IP>::BROADCAST 
			 : ~Traits<IP>::NETMASK | Traits<IP>::ADDRESS));
IP::u16 IP::_next_id = 0;
Simple_List<IP::Protocol_Entry> IP::_ip_observers;
Simple_List<IP::Reassembly_Entry> IP::_reassembly_table;
IP* IP::_instance = 0;

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

Debug& operator<< (Debug &db, const IP::Header &h) {
    IP::Address ip_src(h._src_ip), ip_dst(h._dst_ip);
    IP::u16 flags = h.flags();

    db 	<< "{ver=" << h._version
	<< ",ihl=" << h._ihl
	<< ",tos=" << h._tos
	<< ",len=" << CPU::ntohs(h._length)
	<< ",id="  << CPU::ntohs(h._id)
	<< ",off=" << h.offset()
	<< ",flg=" << (flags == IP::Header::DF_FLAG ? "[DF]" : (flags == IP::Header::MF_FLAG ? "[MF]" : "[ ]"))
	<< ",ttl=" << h._ttl
	<< ",pro=" << h._protocol
	<< ",chk=" << (void *)h._checksum
	<< ",src=" << ip_src
	<< ",dst=" << ip_dst
	<< "}";
    return db;
}

// IP
void IP::attach(Observer &ipo, u16 protocol){
    _ip_observers.insert(&(new Protocol_Entry(protocol, ipo))->_link);
}

void IP::notify(u16 prot, u32 src, void *data, u16 size){
    Simple_List<Protocol_Entry>::Element *e = _ip_observers.head();
    for(;e;e=e->next())
	if(e->object()->_ip_protocol == prot ||
	   e->object()->_ip_protocol == PROT_ANY)
	    e->object()->_o.received(src, data, size);
}

void IP::received(void *data, unsigned int size)
{
    IP::PDU &pck = *reinterpret_cast<IP::PDU*>(data);
    Header &pck_h = pck.header();
    if(pck_h.dst_ip() != _self && pck_h.dst_ip() != BROADCAST){
	db<IP>(INF) << "IP Packet discarded. I'm not the destination.\n";
	return;
    }else
	db<IP>(INF) << "IP " << pck << "\n" ;

    if(pck_h.flags() != Header::MF_FLAG && pck_h.offset() == 0){
	notify(pck_h.protocol(), pck_h.src_ip(), pck.data(), pck_h.length()-pck_h.hlength());
    } else {
	u16 id = pck_h.id();
	Simple_List<Reassembly_Entry>::Element *e = _reassembly_table.head();
	bool found = false;
	for(;e;e=e->next()){
	    if(e->object()->_id == id){
		found = true;
		if(new_fragment(*e->object(), pck)){
		    PDU &pdu = e->object()->_pdu;
		    Header &pdu_h = pdu.header();
		    notify(pdu_h.protocol(), pdu_h.src_ip(), pdu.data(), pdu.size());
		    _reassembly_table.remove(e);
		    delete e->object();
		}
	    }
	}
	if(!found){
	    Reassembly_Entry *e;
	    _reassembly_table.insert(&(e=new Reassembly_Entry(id))->_link);
	    new_fragment(*e,pck);
	}
    }
}

bool IP::get_fragment(u16 i, u16 mtu, void *pdu_, void *frag_){
    PDU &pdu = *reinterpret_cast<PDU*>(pdu_), 
	&frag = *reinterpret_cast<PDU*>(frag_);
    u16 ps = mtu-pdu.header().hlength()-(mtu-pdu.header().hlength())%8;
    u16 data_size = pdu.size() - pdu.header().hlength();
    u16 nf = data_size/ps;
    u16 r;
    nf = (r=data_size%ps) == 0 ? nf-1 : nf;
    if(i>nf) return false;
    bool last = i == nf;
    frag.headercpy(pdu);
    frag.header().set_offset(i*ps/8);
    frag.header().set_length(pdu.header().hlength() + (last ? r : ps));
    frag.header().set_flags(last ? 0 : Header::MF_FLAG);
    frag.set_data_fragment(pdu, i*ps, last ? r : ps);
    frag.header().calculate_checksum();
    db<IP>(INF) << "IP Fragment " << frag << "\n";
    return true;
}


bool IP::new_fragment(Reassembly_Entry &entry, PDU &fragment) {
    PDU &pdu = entry._pdu;
    Reassembly_Entry::List &list = entry._frag_list;
    Header &frag_h = fragment.header();
    u16 offset = frag_h.offset() * 8;
    u16 size   = fragment.size() - frag_h.hlength();

    Reassembly_Entry::Element *m1, *m2, *e = new Reassembly_Entry::Element((char*)offset,size);
    list.insert_merging(e, &m1, &m2);
    memcpy(pdu.data()+offset, fragment.data(), size);

    Header &pdu_h = entry._pdu.header();
    if(frag_h.offset() == 0){
	pdu_h.set_length(frag_h.hlength());
	pdu_h.set_src(frag_h.src_ip());
    }

    if(frag_h.flags() == 0) {
	entry._packet_size = offset + size;
	pdu_h.set_protocol(frag_h.protocol());
	pdu_h.set_offset(0);
	pdu_h.set_length(pdu_h.length()+entry._packet_size);
    }

    if(entry._packet_size == list.grouped_size())
	return true;

    return false;
}

__END_SYS

