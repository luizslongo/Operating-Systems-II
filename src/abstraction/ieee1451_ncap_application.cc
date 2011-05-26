#include <ieee1451_ncap_application.h>

__BEGIN_SYS

IEEE1451_NCAP_Application *IEEE1451_NCAP_Application::_application = 0;

IEEE1451_NCAP_Application::IEEE1451_NCAP_Application()
{
    IEEE1451_Dot0_NCAP::get_instance()->set_application(this);

#ifdef USE_SIP
    SIP_Manager::get_instance()->init();
    SIP_Manager::register_user_handler(message_callback);
#endif
}

IEEE1451_NCAP_Application::~IEEE1451_NCAP_Application()
{
    Simple_List<TIM_Cache>::Iterator it1 = _cache.begin();
    while (it1 != _cache.end())
    {
        Simple_List<TIM_Cache>::Element *el = it1++;
        TIM_Cache *tim_cache = el->object();
        _cache.remove(&tim_cache->_link);
        delete tim_cache;
    }

    Simple_List<IEEE1451_TEDS_Retriever>::Iterator it2 = _retrievers.begin();
    while (it2 != _retrievers.end())
    {
        Simple_List<IEEE1451_TEDS_Retriever>::Element *el = it2++;
        IEEE1451_TEDS_Retriever *retriever = el->object();
        _retrievers.remove(&retriever->_link);
        delete retriever->_tim; //Este tim não foi pra cache ainda!
        delete retriever;
    }
}

IEEE1451_NCAP_Application *IEEE1451_NCAP_Application::get_instance()
{
    if (!_application)
        _application = new IEEE1451_NCAP_Application();
    return _application;
}

IEEE1451_NCAP_Application::TIM_Cache *IEEE1451_NCAP_Application::get_tim_cache(const IP::Address &address)
{
    Simple_List<TIM_Cache>::Iterator it = _cache.begin();
    while (it != _cache.end())
    {
        TIM_Cache *tim_cache = it->object();
        it++;

        if (tim_cache->_tim->_address == address)
        return tim_cache;
    }

    return 0;
}

#ifdef USE_SIP

IEEE1451_NCAP_Application::TIM_Cache *IEEE1451_NCAP_Application::get_tim_cache(const char *uri)
{
    Simple_List<TIM_Cache>::Iterator it = _cache.begin();
    while (it != _cache.end())
    {
        TIM_Cache *tim_cache = it->object();
        it++;

        if (!strcmp(tim_cache->_ua->get_uri(), uri))
            return tim_cache;
    }

    return 0;
}

#endif

IEEE1451_TEDS_Retriever *IEEE1451_NCAP_Application::get_retriever(unsigned short trans_id)
{
    Simple_List<IEEE1451_TEDS_Retriever>::Iterator it = _retrievers.begin();
    while (it != _retrievers.end())
    {
        IEEE1451_TEDS_Retriever *retriever = it->object();
        it++;

        if (retriever->_last_trans_id == trans_id)
            return retriever;
    }

    return 0;
}

void IEEE1451_NCAP_Application::update_tim(const IP::Address &address)
{
    IEEE1451_TEDS_Retriever *retriever = new IEEE1451_TEDS_Retriever(address, this);
    _retrievers.insert(&retriever->_link);
    retriever->execute();
}

void IEEE1451_NCAP_Application::update_tim_completed(IEEE1451_TEDS_Retriever *retriever, IEEE1451_TIM_Channel *tim, IP::Address address)
{
#ifdef USE_SIP
    char uri[100], remote[20], local[20];
    address.to_string(remote);
    ((IP::Address &) IP::instance(0)->address()).to_string(local);
    strcpy(uri, "sip:");
    strcat(uri, remote);
    strcat(uri, "@");
    strcat(uri, local);

    TIM_Cache *tim_cache = new TIM_Cache(tim, SIP_Manager::get_instance()->create_user_agent(uri));
#else
    TIM_Cache *tim_cache = new TIM_Cache(tim);
#endif

    _cache.insert(&tim_cache->_link);

    _retrievers.remove(&retriever->_link);
    delete retriever;

    report_tim_connected(address);

//#ifndef USE_SIP
//	new Thread(&IEEE1451_NCAP_Application::read_data_set_thread, this, address, (IEEE1451_TIM_Channel *) tim);
//#endif
}

void IEEE1451_NCAP_Application::report_tim_connected(const IP::Address &address)
{
    TIM_Cache *tim_cache = get_tim_cache(address);

    if (!tim_cache)
        update_tim(address);
    else
    {
        db<IEEE1451_NCAP_Application>(INF) << "++ TIM CONNECTED (address=" << address << ") ++\n";
        tim_cache->_tim->connect();

#ifdef USE_SIP
        if (tim_cache->_ua->has_subscription())
            send_sip_notify(tim_cache->_ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_OPEN);
#endif

        IEEE1451_Transducer_Channel *transducer = tim_cache->_tim->getTransducer();
        send_operate(address, transducer->get_channel_number());
    }
}

void IEEE1451_NCAP_Application::report_tim_disconnected(const IP::Address &address)
{
    TIM_Cache *tim_cache = get_tim_cache(address);
    if (tim_cache)
    {
        tim_cache->_tim->disconnect();

#ifdef USE_SIP
        if (tim_cache->_ua->has_subscription())
            send_sip_notify(tim_cache->_ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_CLOSED);
#endif

        db<IEEE1451_NCAP_Application>(INF) << "++ TIM DISCONNECTED (address=" << address << ") ++\n";
    }
}

/*void IEEE1451_NCAP_Application::reportError(unsigned short trans_id, int error_code)
{
    IEEE1451_TEDS_Retriever *retriever = get_retriever(trans_id);

    if (retriever)
        retriever->repeat();
    //else
    //  db<IEEE1451_NCAP_Application>(INF) << "++ ERROR (" << trans_id << "): " << error_code << " ++\n";
 }*/

void IEEE1451_NCAP_Application::report_command_reply(const IP::Address &address, unsigned short trans_id, const char *message, unsigned int length)
{
    IEEE1451_TEDS_Retriever *retriever = get_retriever(trans_id);

    if (retriever)
    {
        TEDS_Read_Reply *reply = (TEDS_Read_Reply *) message;
        const char *buffer = message + sizeof(TEDS_Read_Reply);

        if ((!reply->_header._success) || (reply->_header._length < sizeof(reply->_offset)))
        {
            db<IEEE1451_NCAP_Application>(INF) << "Error in report command reply (teds)\n";
            //reportError(trans_id, 0);
            return;
        }

        db<IEEE1451_NCAP_Application>(INF) << "++ REPLY RECEIVED (teds) (trans_id=" << trans_id << ", address=" << address << ", length=" << reply->_header._length << ") ++\n";

        retriever->process(buffer, length - sizeof(TEDS_Read_Reply));
    } else
    {
        Data_Set_Read_Reply *reply = (Data_Set_Read_Reply *) message;
        const char *buffer = message + sizeof(Data_Set_Read_Reply);

        if ((!reply->_header._success) || (reply->_header._length < sizeof(reply->_offset)))
        {
            db<IEEE1451_NCAP_Application>(INF) << "Error in report command reply (data set)\n";
            //reportError(trans_id, 0);
            return;
        }

        db<IEEE1451_NCAP_Application>(INF) << "++ REPLY RECEIVED (data set) (trans_id=" << trans_id << ", address=" << address << ", length=" << reply->_header._length << ") ++\n";

        read_temperature(address, buffer);
    }
}

void IEEE1451_NCAP_Application::report_tim_initiated_message(const IP::Address &address, const char *message, unsigned int length)
{
    Command *cmd = (Command *) message;
    const char *buffer = message + sizeof(Command);

    db<IEEE1451_NCAP_Application>(INF) << "++ TIM MESSAGE RECEIVED (address=" << address << ") ++\n";

    if ((cmd->_channel_number == 1) && (cmd->_command == COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT))
        read_temperature(address, buffer);
}

void IEEE1451_NCAP_Application::read_temperature(const IP::Address &address, const char *buffer)
{
    TIM_Cache *tim_cache = get_tim_cache(address);
    if (!tim_cache)
        return;

    IEEE1451_Transducer_Channel *transducer = tim_cache->_tim->getTransducer();
    IEEE1451_TEDS_NCAP *channel = transducer->get_teds(0x03);

    IEEE1451_TLV *sample = channel->get_tlv(0x12);
    IEEE1451_TEDS_NCAP auxSample(0xff, sample->get_value(), sample->get_length(), true);

    IEEE1451_TLV *dataModel = auxSample.get_tlv(0x28);
    unsigned short model = dataModel->get_value()[0];

    if (model == 1)
    {
        IEEE1451_TLV *dataSet = channel->get_tlv(0x13);
        IEEE1451_TEDS_NCAP auxDataSet(0xff, dataSet->get_value(), dataSet->get_length(), true);

        IEEE1451_TLV *maxDataRepetitions = auxDataSet.get_tlv(0x2b);
        unsigned short repeats = (maxDataRepetitions->get_value()[0] << 8) | maxDataRepetitions->get_value()[1];

        char data[50]; data[0] = 0;
        char aux[11];

        for (unsigned short i = 0, j = 0; i < repeats; i++, j += 4)
        {
            float value = *((float *)(&buffer[j]));

            itoa((int) value, aux);
            strcat(data, aux);
            if (i != (repeats - 1))
                strcat(data, ", ");
        }

        db<IEEE1451_NCAP_Application>(INF) << "Read temperature: " << data << "\n";

#ifdef USE_SIP
        if (tim_cache->_ua->has_subscription())
            send_sip_message(tim_cache->_ua, data);
#endif
    }
}

unsigned short IEEE1451_NCAP_Application::send_operate(const IP::Address &address, unsigned short channel_number)
{
    char *cmd = IEEE1451_Dot0_NCAP::get_instance()->create_command(channel_number, COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE);
    unsigned short trans_id = IEEE1451_Dot0_NCAP::get_instance()->send_command(address, cmd, sizeof(Command));
    delete[] cmd;
    return trans_id;
}

unsigned short IEEE1451_NCAP_Application::send_idle(const IP::Address &address, unsigned short channel_number)
{
    char *cmd = IEEE1451_Dot0_NCAP::get_instance()->create_command(channel_number, COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE);
    unsigned short trans_id = IEEE1451_Dot0_NCAP::get_instance()->send_command(address, cmd, sizeof(Command));
    delete[] cmd;
    return trans_id;
}

unsigned short IEEE1451_NCAP_Application::send_read_teds(const IP::Address &address, unsigned short channel_number, char tedsId)
{
    char *cmd = IEEE1451_Dot0_NCAP::get_instance()->create_command(channel_number, COMMAND_CLASS_READ_TEDS_SEGMENT, &tedsId, 1);
    unsigned short trans_id = IEEE1451_Dot0_NCAP::get_instance()->send_command(address, cmd, sizeof(Command) + 1);
    delete[] cmd;
    return trans_id;
}

unsigned short IEEE1451_NCAP_Application::send_read_data_set(const IP::Address &address, unsigned short channel_number)
{
    unsigned int offset = 0;
    char *cmd = IEEE1451_Dot0_NCAP::get_instance()->create_command(channel_number, COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT, (char *) &offset, sizeof(offset));
    unsigned short trans_id = IEEE1451_Dot0_NCAP::get_instance()->send_command(address, cmd, sizeof(Command) + sizeof(offset));
    delete[] cmd;
    return trans_id;
}

#ifdef USE_SIP

void IEEE1451_NCAP_Application::send_sip_message(SIP_User_Agent *ua, const char *data)
{
    SIP_Request_Message *message = ua->get_uac()->create_message(ua->get_subscriber(), data);
    if (!message)
        return;

    ua->get_uac()->send_request(message);
    //delete message;
}

void IEEE1451_NCAP_Application::send_sip_notify(SIP_User_Agent *ua, SIP_Subscription_State state, SIP_Pidf_Xml_Basic_Element pidfXml)
{
    SIP_Request_Notify *notify = ua->get_uac()->create_notify(ua->get_subscriber(), state, pidfXml, 3600);
    if (!notify)
        return;

    ua->get_uac()->send_request(notify);
    //delete notify;
}

int IEEE1451_NCAP_Application::message_callback(SIP_Event_Callback event, SIP_User_Agent *ua, const char *remote)
{
    IEEE1451_NCAP_Application *app = IEEE1451_NCAP_Application::get_instance();
    TIM_Cache *tim_cache = app->get_tim_cache(ua->get_uri());
    if (!tim_cache)
    {
        db<IEEE1451_NCAP_Application>(INF) << "+++++ Invalid User Agent +++++\n";
        return 1;
    }

    switch (event)
    {
        case SIP_SESSION_INITIATED: //TODO
            db<IEEE1451_NCAP_Application>(INF) << "+++++ Session Initiated +++++\n";
            break;

        case SIP_SESSION_TERMINATED: //TODO
            db<IEEE1451_NCAP_Application>(INF) << "+++++ Session Terminated +++++\n";
            break;

        case SIP_SUBSCRIPTION_INITIATED:
        {
            db<IEEE1451_NCAP_Application>(INF) << "+++++ Subscription Initiated +++++\n";

            if (tim_cache->_tim->connected())
                app->send_sip_notify(ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_OPEN);
            else
                app->send_sip_notify(ua, SIP_SUBSCRIPTION_STATE_ACTIVE, SIP_PIDF_XML_CLOSED);
            break;
        }

        case SIP_SUBSCRIPTION_TERMINATED:
            db<IEEE1451_NCAP_Application>(INF) << "+++++ Subscription Terminated +++++\n";
            break;

        case SIP_MESSAGE_RECEIVED:
        {
            db<IEEE1451_NCAP_Application>(INF) << "+++++ Message Received +++++\n";

            if (!strncmp(ua->get_text_received(), "request data", 12))
                app->send_read_data_set(tim_cache->_tim->_address, 0x01);
            break;
        }

        default:
            break;
    }

    return 0;
}

#endif

/*int IEEE1451_NCAP_Application::read_data_set_thread(IEEE1451_NCAP_Application *ncap, IP::Address address, IEEE1451_TIM_Channel *tim)
{
    db<IEEE1451_NCAP_Application>(INF) << "== NCAP request data thread created (address=" << address << ") ==\n";

    while (1)
    {
        Alarm::delay(2200000);
        if (tim->connected())
        {
            db<IEEE1451_NCAP_Application>(INF) << "-- Reading DataSet (address=" << address << ")...\n";
            ncap->send_read_data_set(address, 0x01);
        }else
            db<IEEE1451_NCAP_Application>(INF) << "-- Address " << address << " is disconnected.\n";
    }
    return 0;
}*/

//-------------------------------------------

IEEE1451_TEDS_Retriever::IEEE1451_TEDS_Retriever(const IP::Address &address, IEEE1451_NCAP_Application *application) :
    _address(address), _application(application), _link(this)
{
    _transducer = new IEEE1451_Transducer_Channel(address, 0x01); //Só suporta um transdutor por mote!
    _tim = new IEEE1451_TIM_Channel(address, _transducer);
    _state = meta_teds;
    _last_trans_id = 0;
}

void IEEE1451_TEDS_Retriever::process(const char *message, unsigned int length)
{
    IEEE1451_TEDS_NCAP *teds = new IEEE1451_TEDS_NCAP(_teds_id, message, length);

    switch (_state)
    {
        case meta_teds:
        case tim_transducer_name_teds:
        case phy_teds:
            _tim->add_teds(teds);
            break;

        case transducer_channel_teds:
        case transducer_name_teds:
            _transducer->add_teds(teds);
            break;

        default:
            delete teds;
            break;
    }

    _state++;
    execute();
}

void IEEE1451_TEDS_Retriever::execute()
{
    unsigned short channel_number = 0xff;

    switch (_state)
    {
        case meta_teds:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> Getting meta teds...\n";
            _teds_id = 0x01;
            channel_number = 0x00;
            break;

        case tim_transducer_name_teds:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> Getting tim's transducer name teds...\n";
            _teds_id = 0x0c;
            channel_number = 0x00;
            break;

        case phy_teds:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> Getting phy teds...\n";
            _teds_id = 0x0d;
            channel_number = 0x00;
            break;

        case transducer_channel_teds:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> Getting transducer channel teds...\n";
            _teds_id = 0x03;
            channel_number = 0x01;
            break;

        case transducer_name_teds:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> Getting transducer's transducer name teds...\n";
            _teds_id = 0x0c;
            channel_number = 0x01;
            break;

        default:
            db<IEEE1451_TEDS_Retriever>(INF) << ">> TEDS completed...\n";
            _teds_id = 0;
            _application->update_tim_completed(this, _tim, _address);
            return;
    }

    if (_teds_id > 0)
        _last_trans_id = _application->send_read_teds(_address, channel_number, _teds_id);
}

__END_SYS
