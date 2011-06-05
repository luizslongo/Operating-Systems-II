#include <ieee1451_ncap.h>

__BEGIN_SYS

//-------------------------------------------

IEEE1451_TEDS_NCAP::IEEE1451_TEDS_NCAP(char id, const char *teds, unsigned short length, bool sub_block) :
    _id(id), _link(this)
{
    unsigned short begin, end;

    if (sub_block)
    {
        begin = 0;
        end = length;
    }else
    {
        begin = 4;
        end = length - 2;
    }

    for (unsigned short i = begin; i < end; )
    {
        unsigned short type = teds[i++] & 0xff;
        unsigned short len = teds[i++] & 0xff;

        char *value = new char[len];
        for (unsigned short j = 0; j < len; j++, i++)
            value[j] = teds[i];

        IEEE1451_TLV *tlv = new IEEE1451_TLV(type, len, value);
        _tlvs.insert(&tlv->_link);
    }
}

IEEE1451_TEDS_NCAP::~IEEE1451_TEDS_NCAP()
{
    Simple_List<IEEE1451_TLV>::Iterator it = _tlvs.begin();
    while (it != _tlvs.end())
    {
        Simple_List<IEEE1451_TLV>::Element *el = it++;
        IEEE1451_TLV *tlv = el->object();
        _tlvs.remove(&tlv->_link);
        delete tlv;
    }
}

IEEE1451_TLV *IEEE1451_TEDS_NCAP::get_tlv(char type)
{
    Simple_List<IEEE1451_TLV>::Iterator it = _tlvs.begin();
    while (it != _tlvs.end())
    {
        IEEE1451_TLV *tlv = it->object();
        it++;

        if (tlv->_type == type)
            return tlv;
    }

    return 0;
}

//-------------------------------------------

IEEE1451_Channel::~IEEE1451_Channel()
{
    Simple_List<IEEE1451_TEDS_NCAP>::Iterator it = _teds.begin();
    while (it != _teds.end())
    {
        Simple_List<IEEE1451_TEDS_NCAP>::Element *el = it++;
        IEEE1451_TEDS_NCAP *teds = el->object();
        _teds.remove(&teds->_link);
        delete teds;
    }
}

IEEE1451_TEDS_NCAP *IEEE1451_Channel::get_teds(char id)
{
    Simple_List<IEEE1451_TEDS_NCAP>::Iterator it = _teds.begin();
    while (it != _teds.end())
    {
        IEEE1451_TEDS_NCAP *teds = it->object();
        it++;

        if (teds->_id == id)
            return teds;
    }

    return 0;
}

//-------------------------------------------

IEEE1451_NCAP *IEEE1451_NCAP::_ieee1451 = 0;

IEEE1451_NCAP::IEEE1451_NCAP() : _application(0), _tcp(IP::instance())
{
    db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - listening...\n";
    NCAP_Socket *socket = new NCAP_Socket(&_tcp);
    _sockets.insert(&socket->_link);
}

IEEE1451_NCAP::~IEEE1451_NCAP()
{
    Simple_List<NCAP_Socket>::Iterator it = _sockets.begin();
    while (it != _sockets.end())
    {
        Simple_List<NCAP_Socket>::Element *el = it++;
        NCAP_Socket *socket = el->object();
        _sockets.remove(&socket->_link);
        delete socket;
    }
}

IEEE1451_NCAP *IEEE1451_NCAP::get_instance()
{
    if (!_ieee1451)
        _ieee1451 = new IEEE1451_NCAP();
    return _ieee1451;
}

char *IEEE1451_NCAP::create_command(unsigned short channel_number, unsigned short command, const char *args, unsigned int length)
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Creating command (type=" << hex << command << ")\n";

    unsigned int size = sizeof(IEEE1451_Command) + length;
    char *buffer = new char[size];

    IEEE1451_Command *cmd = (IEEE1451_Command *) buffer;
    char *msg = buffer + sizeof(IEEE1451_Command);

    cmd->_channel_number = channel_number;
    cmd->_command = command;
    cmd->_length = length;
    memcpy(msg, args, length);

    return buffer;
}

unsigned short IEEE1451_NCAP::send_command(const IP::Address &destination, const char *message, unsigned int length)
{
    static unsigned int id_generator = 1;
    unsigned short trans_id = id_generator++;

    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Sending command (trans_id=" << trans_id << ", dst=" << destination << ")\n";

    NCAP_Socket *socket = get_socket(destination);
    if (!socket)
    {
        db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Failed to send command (trans_id=" << trans_id << ")\n";
        return 0;
    }

    unsigned int size = sizeof(IEEE1451_Packet) + length;
    char buffer[size];

    IEEE1451_Packet *out = (IEEE1451_Packet *) buffer;
    char *msg = buffer + sizeof(IEEE1451_Packet);

    out->_trans_id = trans_id;
    out->_length = length;
    memcpy(msg, message, length);

    socket->send(buffer, size);
    return trans_id;
}

IEEE1451_NCAP::NCAP_Socket *IEEE1451_NCAP::get_socket(const IP::Address &addr)
{
    Simple_List<NCAP_Socket>::Iterator it = _sockets.begin();
    while (it != _sockets.end())
    {
        NCAP_Socket *socket = it->object();
        it++;

        if (socket->remote().ip() == addr)
            return socket;
    }

    return 0;
}

TCP::Socket *IEEE1451_NCAP::NCAP_Socket::incoming(const TCP::Address &from)
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Server socket incoming\n";

    NCAP_Socket *socket = new NCAP_Socket(*this);
    IEEE1451_NCAP::get_instance()->_sockets.insert(&socket->_link);
    return static_cast<TCP::Socket *>(socket);
}

void IEEE1451_NCAP::NCAP_Socket::connected()
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Server socket connected\n";

    IEEE1451_NCAP *ncap = IEEE1451_NCAP::get_instance();
    ncap->_application->report_tim_connected(remote().ip());
}

void IEEE1451_NCAP::NCAP_Socket::closed()
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Server socket closed\n";

    IEEE1451_NCAP *ncap = IEEE1451_NCAP::get_instance();
    ncap->_application->report_tim_disconnected(remote().ip());
    ncap->_sockets.remove(&_link);
    delete this;
}

void IEEE1451_NCAP::NCAP_Socket::received(const char *data, u16 size)
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Server socket received message\n";

    IEEE1451_NCAP *ncap = IEEE1451_NCAP::get_instance();
    IEEE1451_Packet *in = (IEEE1451_Packet *) data;
    const char *msg = data + sizeof(IEEE1451_Packet);

    if (in->_length > 0)
    {
        if (in->_trans_id == 0)
            ncap->_application->report_tim_initiated_message(remote().ip(), msg, in->_length);
        else
            ncap->_application->report_command_reply(remote().ip(), in->_trans_id, msg, in->_length);
    }
}

void IEEE1451_NCAP::NCAP_Socket::closing()
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Server socket closing\n";
    close();
}

__END_SYS
