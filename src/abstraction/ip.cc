#include <ip.h>

__BEGIN_SYS

IP::u16 IP::Header::_next_id = 0;
IP::Route::Hash IP::Route::_table;
IP::ARP::Hash IP::ARP::_table;

NIC * IP::_nic;

IP::IP()
{
    _nic = new (SYSTEM) NIC;

    if(Traits<IP>::CONFIG == Traits<IP>::STATIC)
        new (this) IP(Traits<IP>::ADDRESS, Traits<IP>::NETMASK, Traits<IP>::BROADCAST, Traits<IP>::GATEWAY);
//    else DHCP
}

IP::~IP() {
}

//void IP::process_ip(char *data, u16 size)
//{
//    Header &pck_h = *reinterpret_cast<Header*>(data);
//    if((u32)_self != (u32)0 && // We MUST accept anything if our IP address is not set
//       (u32)(pck_h.dst()) != (u32)(_self) &&
//       (u32)(pck_h.dst()) != (u32)(_broadcast))
//    {
//        db<IP>(INF) << "IP Packet discarded. dst= " << pck_h.dst() << "\n";
//        return;
//    }
//    else {
//        db<IP>(TRC) << "IP: " << pck_h << "\n" ;
//    }
//
//    if(!fragmentation && (pck_h.flags() == Header::MF_FLAG || pck_h.offset() != 0))
//    {
//        db<IP>(INF) << "IP::Fragmented packet discarded\n";
//        return;
//    }
//
//    if (calculate_checksum(data,pck_h.hlength()) != 0xFFFF) {
//        db<IP>(TRC) << "IP checksum failed for incoming packet\n";
//    } else {
//        notify(pck_h.src(),pck_h.dst(),(int)pck_h.protocol(),
//                &data[pck_h.hlength()], pck_h.length() - pck_h.hlength());
//        if (pck_h.ttl() > 0) {
//            pck_h.ttl(pck_h.ttl() - 1);
//        }
//    }
//
//}

int IP::send(const Address & to, const Protocol & prot, const void * data, unsigned int size)
{
//    db<IP>(TRC) << "IP::send(f=" << from <<",t=" << to << ",p=" << prot << ",d=" << data << ",s=" << size << ")" << endl;

    Packet * pkt = new (SYSTEM) Packet(_address, to, prot, data, size);
//    const Route * through = route(to);
//    db<IP>(INF) << "IP::send() => through=" << through << endl;
//    NIC * nic = through->nic();
//    MAC_Address mac = arp(through->gateway());
    NIC * nic = _nic;
    MAC_Address mac = arp(_address);

//    db<IP>(INF) << "IP::send() => gateway=" << through->gateway() << endl;

    unsigned int max_size = sizeof(NIC::Data) - sizeof(Header);

    db<IP>(INF) << "IP::send([pkt=" << pkt << "]) => " << *pkt << endl;

    if(size <= max_size)
        _nic->send(mac, NIC::IP, pkt, sizeof(Header) + size);
    else {
        while(size > 0) {
            if(size > max_size) {
                pkt->header().flags(Header::MF);
                nic->send(mac, NIC::IP, pkt->data(), size);
            } else {
                pkt->header().flags(0);
                nic->send(mac, NIC::IP, pkt->data(), size);
            }
            size -= max_size;
        }
    }

    delete pkt;

    return size;
}

int IP::receive(Address * from, Protocol * prot, void * data, unsigned int size)
{
    IP::MAC_Address nic_src;
    NIC::Protocol nic_pro;
    NIC::Data nic_data;

    _nic->receive(&nic_src, &nic_pro, nic_data, sizeof(Data));
    IP::Packet * pkt = new (nic_data) IP::Packet;

    db<IP>(INF) << "IP::receive([pkt=" << pkt << "]) => " << *pkt << endl;

    *from = pkt->from();
    *prot = pkt->protocol();
    size = pkt->length() - sizeof(Header);
    memcpy(data, pkt->data(), size);

//    unsigned int max_size = sizeof(NIC::Data) - sizeof(Header);

//    if(size <= max_size)
//        _nic->send(mac, NIC::IP, pkt, sizeof(Header) + size);
//    else {
//        while(size > 0) {
//            if(size > max_size) {
//                pkt->header().flags(Header::MF);
//                nic->send(mac, NIC::IP, pkt->data(), size);
//            } else {
//                pkt->header().flags(0);
//                nic->send(mac, NIC::IP, pkt->data(), size);
//            }
//            size -= max_size;
//        }
//    }
//
//    delete pkt;

    db<IP>(TRC) << "IP::receive(f=" << from <<",t=" << _address << ",p=" << prot << ",d=" << data << ",s=" << size << ")" << endl;

    return size;
}
__END_SYS
