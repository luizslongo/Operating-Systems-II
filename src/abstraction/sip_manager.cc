#include <sip_manager.h>
#include <sip_user_agent.h>
#include <utility/random.h>

__BEGIN_SYS

bool SIP_Transport_Layer::init()
{
    _host_port = SIP_PORT;

    char local[20];
    ((IP::Address &) IP::instance(0)->address()).to_string(local);
    _host_ip = create_string(local);
    if (!_host_ip)
    {
        db<SIP_Transport_Layer>(WRN) << "SIP_Transport_Layer::init -> Failed to get the host ip\n";
        return false;
    }

    return true;
}

int SIP_Transport_Layer::send_message(SIP_Message *msg)
{
    char *destination = 0;
    char aux_dest[512];
    int port = 0;

    SIP_Header_Via *via = (SIP_Header_Via *) msg->get_header(SIP_HEADER_VIA);
    if (!via)
        return -1;

    if (msg->get_msg_type() == SIP_RESPONSE)
    {
        destination = (char *) via->get_received();
        if (!destination)
            destination = (char *) via->get_sent_by();
        port = via->get_port();
    } else
    {
        //via->set_sent_by(0, _host_ip);
        //via->set_port(0, _host_port);
        //via->set_transport(0, SIP_TRANSPORT_UDP);

        const char *request_uri = ((SIP_Request *) msg)->get_request_line()->get_request_uri();

        SIP_Header_Route *route = (SIP_Header_Route *) msg->get_header(SIP_HEADER_ROUTE);
        const char *dest = 0;

        if ((route) && (route->is_lr()))
            dest = route->get_address();
        else //if ((!route) || (!route->is_lr()) /*Strict Route*/ )
            dest = request_uri;

        char aux[255];
        strcpy(aux_dest, dest);
        match(aux_dest, ":" , aux);
        skip(aux_dest, " \t");
        match(aux_dest, "@" , aux);
        skip(aux_dest, " \t");
        destination = aux_dest;
        port = 5060;
    }

    if ((!destination) || (port == 0))
        return -1;

    _send_buffer[0] = 0;
    msg->encode(_send_buffer);
    int length = strlen(_send_buffer);

    return send_data(destination, port, _send_buffer, length);
}

int SIP_Transport_Layer::send_data(const char *destination, int port, const char *data, int length)
{
    UDP::Address dst(IP::Address(destination), port);
    _socket.set_remote(dst);

    //db<SIP_Transport_Layer>(INF) << "SIP_Transport_Layer::send_data -> Sending data to " << dst << " (size = " << length << ")..\n" << data << "\n";

    if (_socket.send(data, length) <= 0)
    {
        db<SIP_Transport_Layer>(WRN) << "SIP_Transport_Layer::send_data -> Failed to send data\n";
        return -1;
    }

    return 0;
}

void SIP_Transport_Layer::My_Socket::received(const UDP::Address &src, const char *data, unsigned int size)
{
    db<SIP_Transport_Layer::My_Socket>(INF) << "SIP_Transport_Layer::MySocket::received..\n";

    char remote_addr[20];
    src.ip().to_string(remote_addr);
    int remote_port = src.port();
    ((char *) data)[size] = 0;

    db<SIP_Transport_Layer>(INF) << "SIP_Transport_Layer::received -> Received message from " << remote_addr <<
            ":" << remote_port << " (size = " << size << ")..\n" << data << "\n";

    SIP_Message *msg = SIP_Message::decode_msg(data);

    if (!msg)
    {
        db<SIP_Transport_Layer>(WRN) << "SIP_Transport_Layer::received -> Failed to decode SIP message received\n";
        return;
    }

    bool ok = false;
    SIP_Header_Via *via = (SIP_Header_Via *) msg->get_header(SIP_HEADER_VIA);
    if (via)
    {
        const char *sent_by = via->get_sent_by();
        if (sent_by)
        {
            if (msg->get_msg_type() == SIP_RESPONSE)
            {
                if (!strcmp(sent_by, SIP_Manager::get_instance()->get_transport()->get_host_ip()))
                    ok = true;
            } else
            {
                if (strcmp(sent_by, remote_addr))
                    via->set_received(remote_addr);
                ok = true;
            }
        }
    }

    if (!ok)
    {
        db<SIP_Transport_Layer>(WRN) << "SIP_Transport_Layer::received -> Incorrect message...\n";
        delete msg;
        return;
    }

    SIP_Manager::get_instance()->add_message_list(msg);
}

//-------------------------------------------

USER_CALLBACK SIP_Manager::_callback = 0;
SIP_Manager *SIP_Manager::_instance = 0;
bool SIP_Manager::_terminated = false;

SIP_Manager::SIP_Manager()
{
    _thread_receive_message = 0;
}

SIP_Manager::~SIP_Manager()
{
    _terminated = true;

    if (_thread_receive_message)
    {
        delete _thread_receive_message;
        _thread_receive_message = 0;
    }

    Simple_List<SIP_User_Agent>::Iterator it = _ua.begin();
    while (it != _ua.end())
    {
        SIP_User_Agent *ua = it->object();
        _ua.remove(it++);
        delete ua;
    }
}

SIP_Manager *SIP_Manager::get_instance()
{
    if (!_instance)
    {
        _terminated = false;
        _instance = new SIP_Manager();
    }

    return _instance;
}

void SIP_Manager::delete_instance()
{
    if (_instance)
    {
        delete _instance;
        _instance = 0;
    }
}

bool SIP_Manager::init()
{
    if (!_transport.init())
        return false;

    _thread_receive_message = new Thread(SIP_Manager::receive_message_thread);
    return true;
}

int SIP_Manager::receive_message_thread()
{
    db<SIP_Manager>(INF) << "SIP_Manager::receive_message_thread -> Started..\n";

    SIP_Manager *manager = SIP_Manager::get_instance();

    while (!_terminated)
    {
        while (manager->_messages.size() == 0)
        {
            if (_terminated)
                break;
            Alarm::delay(500);
        }

        if (_terminated)
            break;

        SIP_Message *msg = manager->get_message_list();

        if (!msg)
        {
            db<SIP_Manager>(WRN) << "SIP_Manager::receive_message_thread -> Invalid message\n";
            continue;
        }

        SIP_Message_Type type = msg->get_msg_type();
        db<SIP_Manager>(WRN) << "SIP_Manager::receive_message_thread -> Message received (type=" << type << ")\n";

        if (type == SIP_RESPONSE)
        {
            const char *uri = ((SIP_Header_From *) msg->get_header(SIP_HEADER_FROM))->get_address();
            SIP_User_Agent *ua = manager->get_user_agent(uri);

            if (!ua)
                db<SIP_Manager>(WRN) << "SIP_Manager::receive_message_thread -> Ignoring invalid response\n";
            else
                ua->get_uac()->receive_msg((SIP_Response *) msg);
        } else
        {
            const char *uri = ((SIP_Request *) msg)->get_request_line()->get_request_uri();
            SIP_User_Agent *ua = manager->get_user_agent(uri);

            if (!ua)
                db<SIP_Manager>(WRN) << "SIP_Manager::receive_message_thread -> Ignoring invalid request\n";
            else
                ua->get_uas()->receive_msg((SIP_Request *) msg);
        }

        if (msg->get_can_delete())
            delete msg;
    }

    db<SIP_Manager>(INF) << "SIP_Manager::receive_message_thread -> Stopped..\n";
    return 0;
}

SIP_User_Agent *SIP_Manager::create_user_agent(const char *uri)
{
    SIP_User_Agent *ua = new SIP_User_Agent(uri);
    _ua.insert(&ua->_link);
    return ua;
}

SIP_User_Agent *SIP_Manager::get_user_agent(const char *uri)
{
    Simple_List<SIP_User_Agent>::Iterator it = _ua.begin();
    while (it != _ua.end())
    {
        SIP_User_Agent *ua = it->object();
        it++;

        if (!strcmp(ua->get_uri(), uri))
            return ua;
    }
    return 0;
}

void SIP_Manager::add_message_list(SIP_Message *msg)
{
    _messages_mutex.lock();
    _messages.insert(&msg->_link);
    _messages_mutex.unlock();
}

SIP_Message *SIP_Manager::get_message_list()
{
    _messages_mutex.lock();
    SIP_Message *msg = _messages.head()->object();
    _messages.remove_head();
    _messages_mutex.unlock();
    return msg;
}

void SIP_Manager::random(char *buffer)
{
    unsigned long value = Pseudo_Random::random();
    itoa(value, buffer);
}

__END_SYS
