#include <ieee1451_ncap.h>

__BEGIN_SYS

//-------------------------------------------

IEEE1451_TEDS_NCAP::IEEE1451_TEDS_NCAP(char id, const char *teds, unsigned short length, bool sub_block) :
    _id(id), _length(length), _link(this)
{
    if (!sub_block)
        _length -= 6; //4: length; 2: checksum

    _value = new (kmalloc(_length)) char[_length];
    memcpy(_value, &teds[sub_block ? 0 : 4], _length);
}

char *IEEE1451_TEDS_NCAP::get_tlv(char type)
{
    unsigned short i = 0;
    char t, len;

    while (i < _length)
    {
        t = _value[i];
        len = _value[i + 1];

        if (t == type)
            return &_value[i];

        i += 2 + len;
    }

    return 0;
}

//-------------------------------------------

IEEE1451_Channel::~IEEE1451_Channel()
{
    Simple_List<IEEE1451_TEDS_NCAP>::Iterator it = _teds.begin();
    while (it != _teds.end())
    {
        IEEE1451_TEDS_NCAP *teds = it->object();
        it++;
        _teds.remove_head();
        delete teds;
    }
}

IEEE1451_TEDS_NCAP *IEEE1451_Channel::get_teds(char id)
{
    Simple_List<IEEE1451_TEDS_NCAP>::Iterator it = _teds.begin();
    while (it != _teds.end())
    {
        if (it->object()->_id == id)
            return it->object();
        it++;
    }

    return 0;
}

//-------------------------------------------

Linked_Channel::~Linked_Channel()
{
    if (_send_buffer)
        kfree(_send_buffer);
    if (_receive_buffer)
        kfree(_receive_buffer);
    if (_thread)
        delete _thread;
}

//-------------------------------------------

IEEE1451_NCAP *IEEE1451_NCAP::_ieee1451 = 0;

IEEE1451_NCAP::IEEE1451_NCAP() : _application(0), id_generator(1),
    _udp_socket(UDP::Address(IP::instance()->address(), IEEE1451_PORT),
                UDP::Address(IP::BROADCAST, IEEE1451_PORT))
{
}

IEEE1451_NCAP::~IEEE1451_NCAP()
{
    Simple_List<Linked_Channel>::Iterator it = _tcp_channels.begin();
    while (it != _tcp_channels.end())
    {
        Linked_Channel *chn = it->object();
        it++;
        _tcp_channels.remove_head();
        delete chn;
    }
}

IEEE1451_NCAP *IEEE1451_NCAP::get_instance()
{
    if (!_ieee1451)
        _ieee1451 = new IEEE1451_NCAP();
    return _ieee1451;
}

unsigned short IEEE1451_NCAP::send_command(const IP::Address &destination, unsigned short channel_number, unsigned short command, const char *args, unsigned int length, bool multimedia)
{
    unsigned short trans_id = id_generator++;

    //db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Sending command (trans_id=" << trans_id << ", dst=" << destination << ", chn=" << channel_number << ", cmd=" << hex << command << ", len=" << length << ", media=" << multimedia << ")\n";

    Linked_Channel *channel = get_channel(destination);
    if (!channel)
    {
        db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Failed to send command (trans_id=" << trans_id << ")\n";
        return 0;
    }

    unsigned int size = sizeof(IEEE1451_Packet) + sizeof(IEEE1451_Command) + length;

    IEEE1451_Packet *out = (IEEE1451_Packet *) channel->_send_buffer;
    IEEE1451_Command *cmd = (IEEE1451_Command *) (channel->_send_buffer + sizeof(IEEE1451_Packet));
    char *msg = channel->_send_buffer + sizeof(IEEE1451_Packet) + sizeof(IEEE1451_Command);

    out->_trans_id = trans_id;
    out->_length = length + sizeof(IEEE1451_Command);

    cmd->_channel_number = channel_number;
    cmd->_command = command;
    cmd->_length = length;

    memcpy(msg, args, length);

    int ret;
    if (multimedia)
    {
        _udp_socket.remote(UDP::Address(destination, IEEE1451_PORT));
        ret = _udp_socket.send(channel->_send_buffer, size);
    }else
        ret = channel->send(channel->_send_buffer, size);

    if (ret < 0)
        db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Failed sending message (trans_id=" << trans_id << ", ret=" << ret << ")\n";
    //else
    //  db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Sent " << ret << " bytes (trans_id=" << trans_id << ")\n";

    return trans_id;
}

Linked_Channel *IEEE1451_NCAP::get_channel(const IP::Address &addr)
{
    Simple_List<Linked_Channel>::Iterator it = _tcp_channels.begin();
    while (it != _tcp_channels.end())
    {
        if (it->object()->remote().ip() == addr)
            return it->object();
        it++;
    }

    return 0;
}

void IEEE1451_NCAP::execute()
{
    db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Executing...\n";

    Linked_Channel *channel = new Linked_Channel();
    channel->bind(IEEE1451_PORT);

    while (true)
    {
        db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Listening...\n";

        if (!channel->listen())
        {
            db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Failed listening\n";
            continue;
        }

        db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - New channel connected (ip=" << channel->remote().ip() << ")\n";

        Linked_Channel *chn = get_channel(channel->remote().ip());
        if (chn)
        {
            db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Deleting old channel...\n";
            _tcp_channels.remove(&chn->_link);
            chn->Linked_Channel::~Linked_Channel();
            delete chn;
        }

        _tcp_channels.insert(&channel->_link);
        channel->_send_buffer = new (kmalloc(MAX_BUFFER_SIZE)) char[MAX_BUFFER_SIZE];
        channel->_receive_buffer = new (kmalloc(MAX_BUFFER_SIZE)) char[MAX_BUFFER_SIZE];
        channel->_thread = new Thread(receive, this, channel, Thread::READY, Thread::NORMAL, 1100);

        channel = new Linked_Channel();
        channel->bind(IEEE1451_PORT);
    }
}

int IEEE1451_NCAP::receive(IEEE1451_NCAP *ncap, Linked_Channel *channel)
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Receive thread created (ip=" << channel->remote().ip() << ")\n";

    IEEE1451_Packet *in;
    const char *msg;
    int ret;

#ifdef __mc13224v__
    Alarm::delay(TIME_500_MS * 2);
#endif

    ncap->_application->report_tim_connected(channel->remote().ip());

#ifdef __mc13224v__
    Alarm::delay(TIME_500_MS * 2);
#endif

    while (true)
    {
        db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Receiving...\n";
        ret = channel->receive(channel->_receive_buffer, MAX_BUFFER_SIZE);

        if (ret < 0)
            break;

        if (ret < (int) sizeof(IEEE1451_Packet))
            continue;

        in = (IEEE1451_Packet *) channel->_receive_buffer;
        msg = channel->_receive_buffer + sizeof(IEEE1451_Packet);

        if (in->_length > 0)
        {
            if (in->_trans_id == 0)
                ncap->_application->report_tim_initiated_message(channel->remote().ip(), msg, in->_length);
            else
                ncap->_application->report_command_reply(channel->remote().ip(), in->_trans_id, msg, in->_length);
        }
    }

#ifdef __mc13224v__
    Alarm::delay(TIME_500_MS * 2);
#endif

    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Closing channel (ip=" << channel->remote().ip() << ")...\n";

    while (!channel->close())
        Alarm::delay(TIME_500_MS);

    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Channel closed (ip=" << channel->remote().ip() << ")\n";

    ncap->_application->report_tim_disconnected(channel->remote().ip());
    ncap->_tcp_channels.remove(&channel->_link);

    Functor_Handler<Linked_Channel> handler(&cleaner, channel);
    Alarm alarm(TIME_500_MS, &handler, 1);

    db<IEEE1451_TIM>(TRC) << "IEEE1451_NCAP - Receive thread finished (ip=" << channel->remote().ip() << ")\n";
    Thread::self()->suspend();
    return 0;
}

void IEEE1451_NCAP::cleaner(Linked_Channel *channel)
{
    channel->Linked_Channel::~Linked_Channel();
    delete channel;
}

void IEEE1451_NCAP::UDP_Socket::received(const UDP::Address &src, const char *data, unsigned int size)
{
    if (size < sizeof(IEEE1451_Packet))
        return;

    IEEE1451_Packet *in = (IEEE1451_Packet *) data;
    const char *msg = data + sizeof(IEEE1451_Packet);

    if (in->_length > 0)
    {
        if (in->_trans_id == 0)
            IEEE1451_NCAP::get_instance()->_application->report_tim_initiated_message(src.ip(), msg, in->_length);
        else
            IEEE1451_NCAP::get_instance()->_application->report_command_reply(src.ip(), in->_trans_id, msg, in->_length);
    }
}

__END_SYS
