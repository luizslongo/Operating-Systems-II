#include <ieee1451_ncap.h>
#include <ieee1451_ncap_application.h>

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
    } else
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

        if (tlv->get_type() == type)
            return tlv;
    }

    return 0;
}

//-------------------------------------------

//const unsigned short IEEE1451_TIM_Channel::_tim_channel_number = 0;

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

IEEE1451_Dot0_NCAP *IEEE1451_Dot0_NCAP::_dot0 = 0;

IEEE1451_Dot0_NCAP::IEEE1451_Dot0_NCAP()
{
    _application = 0;
    IEEE1451_Dot5_NCAP::get_instance();
}

IEEE1451_Dot0_NCAP *IEEE1451_Dot0_NCAP::get_instance()
{
    if (!_dot0)
        _dot0 = new IEEE1451_Dot0_NCAP();
    return _dot0;
}

char *IEEE1451_Dot0_NCAP::create_command(unsigned short channel_number, unsigned short command, const char *args, unsigned int length)
{
    unsigned int size = sizeof(IEEE1451_Command) + length;
    char *buffer = new char[size];
    char *msg = buffer + sizeof(IEEE1451_Command);

    IEEE1451_Command *cmd = (IEEE1451_Command *) buffer;
    cmd->_channel_number = channel_number;
    cmd->_command = command;
    cmd->_length = length;
    memcpy(msg, args, length);

    return buffer;
}

unsigned short IEEE1451_Dot0_NCAP::send_command(const IP::Address &destination, const char *message, unsigned int length)
{
    static unsigned int id_generator = 1;
    unsigned short trans_id = id_generator++;

    IEEE1451_Dot5_NCAP::get_instance()->send_msg(trans_id, destination, message, length);
    return trans_id;
}

void IEEE1451_Dot0_NCAP::tim_connected(const IP::Address &address)
{
    _application->report_tim_connected(address);
}

void IEEE1451_Dot0_NCAP::tim_disconnected(const IP::Address &address)
{
    _application->report_tim_disconnected(address);
}

void IEEE1451_Dot0_NCAP::receive_msg(const IP::Address &address, unsigned short trans_id, const char *message, unsigned int length)
{
    if (trans_id == 0)
        _application->report_tim_initiated_message(address, message, length);
    else
        _application->report_command_reply(address, trans_id, message, length);
}

/*void IEEE1451_Dot0_NCAP::error_on_send(int error_code, unsigned short trans_id)
{
    _application->report_error(trans_id, error_code);
}*/

//-------------------------------------------

IEEE1451_Dot5_NCAP *IEEE1451_Dot5_NCAP::_dot5 = 0;

IEEE1451_Dot5_NCAP::IEEE1451_Dot5_NCAP() : _tcp(IP::instance())
{
    db<IEEE1451_Dot5_NCAP>(INF) << "IEEE1451dot5 listening...\n";
    My_Server_Socket *socket = new My_Server_Socket(&_tcp);
    _sockets.insert(&socket->_link);
}

IEEE1451_Dot5_NCAP::~IEEE1451_Dot5_NCAP()
{
    Simple_List<My_Server_Socket>::Iterator it = _sockets.begin();
    while (it != _sockets.end())
    {
        Simple_List<My_Server_Socket>::Element *el = it++;
        My_Server_Socket *socket = el->object();
        _sockets.remove(&socket->_link);
        delete socket;
    }
}

IEEE1451_Dot5_NCAP *IEEE1451_Dot5_NCAP::get_instance()
{
    if (!_dot5)
        _dot5 = new IEEE1451_Dot5_NCAP();
    return _dot5;
}

IEEE1451_Dot5_NCAP::My_Server_Socket *IEEE1451_Dot5_NCAP::get_socket(const IP::Address &addr)
{
    Simple_List<My_Server_Socket>::Iterator it = _sockets.begin();
    while (it != _sockets.end())
    {
        My_Server_Socket *socket = it->object();
        it++;

        if (socket->remote().ip() == addr)
            return socket;
    }

    return 0;
}

void IEEE1451_Dot5_NCAP::send_msg(unsigned short trans_id, const IP::Address &destination, const char *message, unsigned int length)
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Sending message (trans_id=" << trans_id << ")...\n";

    My_Server_Socket *socket = get_socket(destination);
    if (!socket)
    {
        db<IEEE1451_Dot5_NCAP>(INF) << "Failed to send message (trans_id=" << trans_id << ")\n";
        return;
    }

    unsigned int data_length = sizeof(IEEE1451_Packet) + length;
    char data[data_length];

    IEEE1451_Packet *out = (IEEE1451_Packet *) data;
    char *msg = data + sizeof(IEEE1451_Packet);

    out->_trans_id = trans_id;
    out->_length = length;
    memcpy(msg, message, length);

    socket->send(data, data_length);
}

TCP::Socket *IEEE1451_Dot5_NCAP::My_Server_Socket::incoming(const TCP::Address &from)
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Server socket incoming\n";
    My_Server_Socket *socket = new My_Server_Socket(*this);
    IEEE1451_Dot5_NCAP::get_instance()->_sockets.insert(&socket->_link);
    return static_cast<TCP::Socket *>(socket);
}

void IEEE1451_Dot5_NCAP::My_Server_Socket::connected()
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Server socket connected\n";
    IEEE1451_Dot0_NCAP::get_instance()->tim_connected(remote().ip());
}

void IEEE1451_Dot5_NCAP::My_Server_Socket::closed()
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Server socket closed\n";
    IEEE1451_Dot0_NCAP::get_instance()->tim_disconnected(remote().ip());
    IEEE1451_Dot5_NCAP::get_instance()->_sockets.remove(&_link);
    delete this;
}

void IEEE1451_Dot5_NCAP::My_Server_Socket::received(const char *data, u16 size)
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Server socket received message\n";
    IEEE1451_Packet *in = (IEEE1451_Packet *) data;
    const char *msg = data + sizeof(IEEE1451_Packet);

    if (in->_length > 0)
        IEEE1451_Dot0_NCAP::get_instance()->receive_msg(remote().ip(), in->_trans_id, msg, in->_length);
}

void IEEE1451_Dot5_NCAP::My_Server_Socket::closing()
{
    db<IEEE1451_Dot5_NCAP>(INF) << "Server socket closing\n";
    close();
}

__END_SYS
