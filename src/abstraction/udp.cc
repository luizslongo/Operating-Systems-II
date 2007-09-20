#include <udp.h>

__BEGIN_SYS

Simple_List<UDP::Receive_Entry> UDP::_receive_list;
Simple_Ordered_List<UDP, UDP::u16> UDP::_busy_ports;
UDP::u16 UDP::_port = 1024;

void UDP::PDU::calculate_checksum(){
	if(/*Traits<UDP>::checksum*/ true) {
		Pseudo_Header phdr;
		phdr.src_ip = _ip_header.src_ip();
		phdr.dst_ip = _ip_header.dst_ip();
		phdr.zero = 0;
		phdr.protocol = ID;
		phdr.length = CPU::htons(size());

		unsigned int sum = 0;

		u16 * ptr = reinterpret_cast<u16 *>(this);
		for(unsigned int i = 0; i < size() / sizeof(u16); i++)
			sum += ptr[i];
		if(size() & 1)
			sum += ptr[size()] << 8;

		ptr = reinterpret_cast<u16 *>(&phdr);
		for(unsigned int i = 0;
				i < sizeof(Pseudo_Header) / sizeof(u16); i++)
			sum += ptr[i];

		while(sum >> 16)
			sum = (sum & 0xffff) + (sum >> 16);

		_udp_header.checksum(~sum);
	} else
		_udp_header.checksum(0);
}

void UDP::constructor_common(){

	// To find an available port
	// Checks one port above and the next below the desidered port, respecting the limits
        u16 desired = _self.port(); 
        int i, j;	i = j = 0; 
        bool min, max;	min = max = false; 
        while(_busy_ports.search_rank(desired+i)){ 
                bool even;       
                do{ 
                        even = ++j%2 == 0; 
                        i = even ? -j>>1 : (j+1)>>1; 
                        if(!even && !max && desired+i > 0xffff) max = true; 
                        if( even && !min && desired+i < 0)      min = true; 
                } while((even && min || !even && max) && !(min && max)); 
 
                if(max && min) {  
			db<UDP>(ERR) << "No ports available. Inconsistent State.\n";
                        break; 
                }  
        }

	if(i){ 
		_self.change_port(desired + i);
		db<UDP>(ERR) << "Port " << desired << " is busy. Channel created "
			"on the next available port: " << (desired+i) << "\n";
	}

	_link = new Ports_List::Element(this, _self.port());

	IP::attach(ID_UDP, *this);
	_busy_ports.insert(_link);

	db<UDP>(TRC) << "UDP::UDP() self= " << _self << "\n";
}

void UDP::received(u32 src, void *data, u16  size){
	size -= sizeof(UDP::Header);

	Header &h = *reinterpret_cast<Header*>(data);
	Simple_List<Receive_Entry>::Element *e = _receive_list.head();
	bool found = false;
	for(;e;e=e->next())
		if(e->object()->_port == h.dst_port()){
			found = true;
			Receive_Entry &r = *e->object();
			r._size = size < r._size ? size : r._size;
			Address source(src, h.src_port());
			r._src = source;
			memcpy(r._data, ((char*)data)+sizeof(UDP::Header), r._size);
			r._s.v();
			break;
		}

	if(!found) 
		db<UDP>(INF) 	<< "INVALID PORT (" << h.dst_port() 
				<< ") OR RECEIVE NOT CALLED - Packet discarded\n";
}

int UDP::receive(UDP::Address &src, void *data, unsigned int size){
	Receive_Entry *re;
	_receive_list.insert(&(re=new Receive_Entry(_self.port(), _semaph, src, data, size))->_link);
	db<UDP>(INF) << "Port " << re->_port << " has called receive\n";
	_semaph.p();
	_receive_list.remove(re);
	int s = re->_size;
	delete re;
	return s;
}


__END_SYS

