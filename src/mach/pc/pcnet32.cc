// EPOS PC AMD PCNet II (Am79C970A) Ethernet NIC Mediator Implementation

#include <mach/pc/machine.h>
#include <mach/pc/pcnet32.h>
#include <utility/malloc.h>

__BEGIN_SYS

// Class attributes
PCNet32::Device PCNet32::_devices[UNITS];


// Methods
PCNet32::PCNet32(unsigned int unit)
{
    db<PCNet32>(TRC) << "PCNet32(unit=" << unit << ")" << endl;

    if(unit >= UNITS) {
        db<PCNet32>(WRN) << "PCNet32: requested unit (" << unit << ") does not exist!" << endl;
        return;
    }

    *this = *_devices[unit].device;
}


PCNet32::~PCNet32()
{
    db<PCNet32>(TRC) << "~PCNet32(unit=" << _unit << ")" << endl;
}


void PCNet32::reset()
{
    // Reset the device
    s_reset();

    // Software style => PCI, 32 bits, burst mode (pg 147)
    bcr(20, BCR20_SSIZE32 | BCR20_SWSTYLE2);

    // Get MAC address from PROM
    _address = Address(prom(0), prom(1), prom(2), prom(3), prom(4), prom(5));
    db<PCNet32>(INF) << "PCNet32::reset: MAC=" << _address << endl;

    // Enable auto-select port
    bcr(2, BCR2_ASEL);

    // Enable full-duplex
    bcr(9, BCR9_FDEN);

    // Disable INIT interrupt and transmit stop on underflow and two part deferral
    csr(3, CSR3_TINTM | CSR3_IDONM | CSR3_DXMT2PD | CSR3_LAPPEN | CSR3_DXSUFLO);

    // Enable frame auto padding/stripping and auto CRC handling
    csr(4, CSR4_DMAPLUS | CSR4_DPOLL | CSR4_APAD_XMT | CSR4_ASTRP_RCV | CSR4_TXSTRTM);

    // Adjust interrupts
    csr(5, CSR5_TOKINTD | CSR5_SINTE | CSR5_EXDINTE);

    // Enable burst read and write
    bcr(18, bcr(18) | BCR18_BREADE | BCR18_BWRITE);

    // Promiscuous mode
    csr(15, csr(15) | CSR15_PROM);
        
    // Set transmit start point to full frame	
    csr(80, csr(80) | 0x0c00); // XMTSP = 11

    // Setup a init block
    _iblock->mode = 0x0000;
    _iblock->rlen = log2(RX_BUFS) << 4;
    _iblock->tlen = log2(TX_BUFS) << 4;
    _iblock->mac_addr = _address;        
    _iblock->filter1 = 0;             
    _iblock->filter2 = 0;             
    _iblock->rx_ring = _rx_ring_phy;
    _iblock->tx_ring = _tx_ring_phy;
    csr(1, _iblock_phy & 0xffff); 
    csr(2, _iblock_phy >> 16); 

    // Initialize the device
    csr(0, CSR0_IENA | CSR0_INIT);
    for(int i = 0; (i < 100) && !(csr(0) & CSR0_IDON); i++);
    if(!(csr(0) & CSR0_IDON))
        db<PCNet32>(WRN) << "PCNet32::reset: initialization failed!" << endl;

    // Get MAC address from CSR
    csr(0, CSR0_IDON | CSR0_STOP);
    Address csr_addr(csr(PADR0), csr(PADR1), csr(PADR2));

    if(_address != csr_addr) {
        db<PCNet32>(WRN) << "PCNet32::reset: initialization failed!" << endl;
        db<PCNet32>(WRN) << "PCNet32::reset: MAC(ROM)=" << _address << endl;
        db<PCNet32>(WRN) << "PCNet32::reset: MAC(CSR)=" << csr_addr << endl;
    }

    // Activate sending and receiving 
    csr(0, CSR0_IENA | CSR0_STRT);
}


int PCNet32::send(const Address & dst, const Protocol & prot, const void * data, unsigned int size)
{
    // Wait for a free buffer and seize it
    for(; _tx_ring[_tx_cur].status & Tx_Desc::OWN; ++_tx_cur %= TX_BUFS)
        if(_tx_ring[_tx_cur].lock())
            break;

    Tx_Desc * desc = &_tx_ring[_tx_cur];
    Buffer * buf = _tx_buffer[_tx_cur];
    ++_tx_cur %= TX_BUFS;

    db<PCNet32>(TRC) << "PCNet32::send(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec
                     << ",d=" << data << ",s=" << size << ")" << endl;

    // Assemble the Ethernet frame
    new (buf->frame()) Frame(_address, dst, prot, data, size);

    desc->size = -(size + sizeof(Header)); // 2's comp.

    // Status must be set last, since it can trigger a send
    desc->status = Tx_Desc::OWN | Tx_Desc::STP | Tx_Desc::ENP;

    db<PCNet32>(INF) << "PCNet32::send([desc=" << desc << "]) => " << *reinterpret_cast<Desc *>(desc) << endl;

    desc->unlock();

    // Trigger an immediate send poll
    csr(0, csr(0) | CSR0_TDMD);

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    return size;
}

int PCNet32::send(const Address & dst, const Protocol & prot, Buffer * buf)
{
    unsigned int size = buf->size();
    Tx_Desc * desc = reinterpret_cast<Tx_Desc *>(buf->back());

    db<PCNet32>(TRC) << "PCNet32::send(s=" << _address << ",d=" << dst << ",p=" << hex << prot << dec
                     << ",b=" << buf << ",s=" << size << ")" << endl;

    // Assemble the Ethernet frame
    new (buf->frame()) Frame(_address, dst, prot); // Data is already in buf!

    desc->size = -(size + sizeof(Header)); // 2's comp.

    // Status must be set last, since it can trigger a send
    desc->status = Tx_Desc::OWN | Tx_Desc::STP | Tx_Desc::ENP;

    db<PCNet32>(INF) << "PCNet32::send([desc=" << desc << "]) => " << *reinterpret_cast<Desc *>(desc) << endl;

    desc->unlock();

    // Trigger an immediate send poll
    csr(0, csr(0) | CSR0_TDMD);

    _statistics.tx_packets++;
    _statistics.tx_bytes += size;

    return size;
}


int PCNet32::receive(Address * src, Protocol * prot, void * data, unsigned int size)
{
    // Wait for a received frame
    for(; _rx_ring[_rx_cur].status & Rx_Desc::OWN; ++_rx_cur %= RX_BUFS);

    Buffer * buf = _rx_buffer[_rx_cur];
    Rx_Desc * desc = &_rx_ring[_rx_cur];
    ++_rx_cur %= RX_BUFS;

    // Disassemble the Ethernet frame
    Frame * frame = buf->frame();
    *src = frame->src();
    *prot = frame->prot();
    buf->size((desc->misc & 0x00000fff) - sizeof(Header) - sizeof(CRC));

    // Copy data
    memcpy(data, frame->data(), (buf->size() > size) ? size : buf->size());
 
    // Release the buffer to the NIC
    desc->status = Rx_Desc::OWN;

    _statistics.rx_packets++;
    _statistics.rx_bytes += buf->size();

    db<PCNet32>(TRC) << "PCNet32::receive(s=" << *src << ",p=" << hex << *prot << dec
        	     << ",d=" << data << ",s=" << buf->size() << ") => " << endl;

    return buf->size();
}


PCNet32::Buffer * PCNet32::alloc(unsigned int once, unsigned int always, unsigned int payload)
{
    db<PCNet32>(TRC) << "PCNet32::alloc(o=" << once << ",a=" << always << ",p=" << payload << ")" << endl;

    // Calculate how many frames are needed to hold the transport PDU
    int size = once + payload;
    unsigned int max_data = MTU - always;
    unsigned int frames = 0;
    while(size> 0) {
        frames++;
        size -= max_data;
    }

    if(!frames)
        return 0;

    if(frames > TX_BUFS)
        db<PCNet32>(WRN) << "PCNet32::alloc(): sizeof(Network::Packet::Data) > sizeof(NIC::Frame::Data) * RX_BUFS!" << endl;

    // Wait for a free buffer and seize it
    for(; _tx_ring[_tx_cur].status & Tx_Desc::OWN; ++_tx_cur %= TX_BUFS)
        if(_tx_ring[_tx_cur].lock())
            break;

    // The head buffer holds the pool size
    Buffer * pool = _tx_buffer[_tx_cur];
    pool->size(once + payload);
    ++_tx_cur %= TX_BUFS;
    frames--;

    Buffer * buf = pool;
    for(; frames; ++_tx_cur %= TX_BUFS) {
        if(!(_tx_ring[_tx_cur].status & Tx_Desc::OWN) & _tx_ring[_tx_cur].lock()) {
            buf->next(_tx_buffer[_tx_cur]);
            buf = buf->next();
            frames--;
        }
    }
    buf->next(0);

    for(buf = pool; buf; buf = buf->next())
        db<PCNet32>(INF) << "PCNet32::alloc() => {buf=" << buf << ",desc=" << buf->back() << "}" << endl;

    return pool;
}


void PCNet32::free(Buffer * buf)
{
    db<PCNet32>(TRC) << "PCNet32::free(b=" << buf << ")" << endl;

    // Release the buffer to the NIC
    Rx_Desc * desc = reinterpret_cast<Rx_Desc *>(buf->back());
    desc->unlock();
    desc->status = Rx_Desc::OWN;
}


void PCNet32::handle_int()
{
    if(csr(0) & CSR0_INTR) {
        int csr0 = csr(0);
        int csr4 = csr(4);
        int csr5 = csr(5);

        // Clear interrupts
        csr(0, csr0);
        csr(4, csr4);
        csr(5, csr5);

 	if(csr0 & CSR0_RINT) { // Frame received
 	    Buffer * buf = _rx_buffer[_rx_cur];
            Rx_Desc * desc = &_rx_ring[_rx_cur];
            Frame * frame = buf->frame();
            ++_rx_cur %= RX_BUFS;

            Protocol prot = frame->header()->prot(); // will be overwritten by buf->size()
            buf->size((desc->misc & 0x00000fff) - sizeof(Header) - sizeof(CRC));

            _statistics.rx_packets++;
            _statistics.rx_bytes += buf->size();

            db<PCNet32>(TRC) << "PCNet32::int:receive(s=" << frame->src() << ",p=" << hex << prot << dec
                             << ",d=" << frame->data() << ",s=" << buf->size() << ")" << endl;

            notify(prot, buf);
 	}

        if(csr0 & CSR0_ERR) { // Error
            db<PCNet32>(INF) << "PCNet32::int:error =>";

            if(csr0 & CSR0_MERR) { // Memory
        	db<PCNet32>(WRN) << " memory";
            }
            
            if(csr0 & CSR0_MISS) { // Missed Frame
        	db<PCNet32>(WRN) << " missed frame";
        	_statistics.rx_overruns++;
            }

            if(csr0 & CSR0_CERR) { // Collision
        	db<PCNet32>(INF) << " collision";
        	_statistics.collisions++;
            }

            if(csr0 & CSR0_BABL) { // Bable transmitter time-out
        	db<PCNet32>(INF) << " overrun";
        	_statistics.tx_overruns++;
            }

            db<PCNet32>(INF) << endl;
        }
    }
}


void PCNet32::int_handler(unsigned int interrupt)
{
    PCNet32 * dev = get(interrupt);

    db<PCNet32>(TRC) << "PCNet32::int_handler(int=" << interrupt << ",dev=" << dev << ")" << endl;

    if(!dev)
        db<PCNet32>(WRN) << "PCNet32::int_handler: handler not assigned!" << endl;
    else
        dev->handle_int();
}

__END_SYS
