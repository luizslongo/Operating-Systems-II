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

IEEE1451_NCAP *IEEE1451_NCAP::_ieee1451 = 0;

IEEE1451_NCAP::IEEE1451_NCAP() : _application(0), id_generator(1)
{
    _send_buffer = new (kmalloc(MAX_BUFFER_SIZE)) char[MAX_BUFFER_SIZE];
}

IEEE1451_NCAP::~IEEE1451_NCAP()
{
    Simple_List<Linked_Channel>::Iterator it = _channels.begin();
    while (it != _channels.end())
    {
        Linked_Channel *chn = it->object();
        it++;
        _channels.remove_head();
        delete chn;
    }

    if (_send_buffer)
        kfree(_send_buffer);
}

IEEE1451_NCAP *IEEE1451_NCAP::get_instance()
{
    if (!_ieee1451)
        _ieee1451 = new IEEE1451_NCAP();
    return _ieee1451;
}

unsigned short IEEE1451_NCAP::send_command(const IP::Address &destination, unsigned short channel_number, unsigned short command, const char *args, unsigned int length)
{
    _send_buffer_mutex.lock();

    unsigned short trans_id = id_generator++;

    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Sending command (trans_id=" << trans_id << ", dst=" << destination << ", chn=" << channel_number << ", cmd=" << hex << command << ", len=" << length << ")\n";

    Linked_Channel *channel = get_channel(destination);
    if (!channel)
    {
        db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Failed to send command (trans_id=" << trans_id << ")\n";
        _send_buffer_mutex.unlock();
        return 0;
    }

    unsigned int size = sizeof(IEEE1451_Packet) + sizeof(IEEE1451_Command) + length;

    IEEE1451_Packet *out = (IEEE1451_Packet *) _send_buffer;
    IEEE1451_Command *cmd = (IEEE1451_Command *) (_send_buffer + sizeof(IEEE1451_Packet));
    char *msg = _send_buffer + sizeof(IEEE1451_Packet) + sizeof(IEEE1451_Command);

    out->_trans_id = trans_id;
    out->_length = length + sizeof(IEEE1451_Command);

    cmd->_channel_number = channel_number;
    cmd->_command = command;
    cmd->_length = length;

    memcpy(msg, args, length);

#ifdef __mc13224v__
    Alarm::delay(TIME_500_MS * 4);
#endif

    int ret = channel->send(_send_buffer, size);

    if (ret < 0)
        db<IEEE1451_NCAP>(INF) << "IEEE1451_NCAP - Failed sending message (trans_id=" << trans_id << ", ret=" << ret << ")\n";
    else
        db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Sent " << ret << " bytes (trans_id=" << trans_id << ")\n";

    _send_buffer_mutex.unlock();
    return trans_id;
}

Linked_Channel *IEEE1451_NCAP::get_channel(const IP::Address &addr)
{
    Simple_List<Linked_Channel>::Iterator it = _channels.begin();
    while (it != _channels.end())
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

    Linked_Channel *channel = new (kmalloc(sizeof(Linked_Channel))) Linked_Channel();
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
            _channels.remove(&chn->_link);
            chn->TCP::Channel::~Channel();
            kfree(chn);
        }

        _channels.insert(&channel->_link);
        new Thread(receive, this, channel);

        channel = new (kmalloc(sizeof(Linked_Channel))) Linked_Channel();
        channel->bind(IEEE1451_PORT);
    }
}

int IEEE1451_NCAP::receive(IEEE1451_NCAP *ncap, Linked_Channel *channel)
{
    db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Receive thread created (ip=" << channel->remote().ip() << ")\n";

    char *_receive_buffer = new (kmalloc(MAX_BUFFER_SIZE)) char[MAX_BUFFER_SIZE];
    IEEE1451_Packet *in;
    const char *msg;
    int ret;

#ifdef __mc13224v__
    Alarm::delay(TIME_500_MS * 4);
#endif

    ncap->_application->report_tim_connected(channel->remote().ip());

    while (true)
    {
#ifdef __mc13224v__
        Alarm::delay(TIME_500_MS * 4);
#endif

        db<IEEE1451_NCAP>(TRC) << "IEEE1451_NCAP - Receiving...\n";
        ret = channel->receive(_receive_buffer, MAX_BUFFER_SIZE);

        if (ret < 0)
            break;

        if (ret < (int) sizeof(IEEE1451_Packet))
            continue;

        in = (IEEE1451_Packet *) _receive_buffer;
        msg = _receive_buffer + sizeof(IEEE1451_Packet);

#ifdef __mc13224v__
        Alarm::delay(TIME_500_MS * 4);
#endif

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

    ncap->_channels.remove(&channel->_link);
    channel->TCP::Channel::~Channel();
    kfree(channel);
    kfree(_receive_buffer);

    char handler_alloc[sizeof(Functor_Handler<Thread>)];
    char alarm_alloc[sizeof(Alarm)];

    Functor_Handler<Thread> *handler = new (&handler_alloc) Functor_Handler<Thread>(&cleaner, Thread::self());
    new (&alarm_alloc) Alarm(TIME_500_MS, handler, 1);

    db<IEEE1451_TIM>(TRC) << "IEEE1451_NCAP - Receive thread finished (ip=" << channel->remote().ip() << ")\n";
    Thread::self()->suspend();
    return 0;
}

__END_SYS
