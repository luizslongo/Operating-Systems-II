#include <ieee1451_tim.h>

__BEGIN_SYS

IEEE1451_Transducer::IEEE1451_Transducer()
{
    IEEE1451_Dot0_TIM::get_instance()->setTransducer(this);
}

void IEEE1451_Transducer::receive_msg(unsigned short trans_id, const char *message, unsigned int size)
{
    Command *cmd = (Command *) message;
    const char *buffer = message + sizeof(Command);

    switch (cmd->_command)
    {
        /*case COMMAND_CLASS_QUERY_TEDS:
         case COMMAND_CLASS_UPDATE_TEDS:
         {
             TEDS *teds = get_teds(buffer[0]);
             TEDS_Query_Reply reply;
             reply._header._length = 12;
             if (teds)
             {
                 reply._maxSize = 0;
                 reply._checksum = *((unsigned short *) teds->get_payload());
                 reply._size = teds->get_size();
                 reply._status = teds->get_status();
                 reply._atributes = teds->get_atributes();
                 reply._header._success = true;
             }else
             {
                 reply._maxSize = 0;
                 reply._checksum = 0;
                 reply._size = 0;
                 reply._status = 0;
                 reply._atributes = 0x40;
                 reply._header._success = false;
             }

             IEEE1451_Dot5_TIM::get_instance()->send_msg(trans_id, (char *) &reply, sizeof(reply));
             break;
         }*/

        case COMMAND_CLASS_READ_TEDS_SEGMENT:
        {
            IEEE1451_TEDS_TIM *teds = get_teds(buffer[0]);
            if (teds)
            {
                unsigned int teds_size = teds->get_size();
                unsigned int reply_size = sizeof(TEDS_Read_Reply) + teds_size;
                char *reply = new char[reply_size];

                TEDS_Read_Reply *read_reply = (TEDS_Read_Reply *) reply;
                read_reply->_header._success = true;
                read_reply->_header._length = teds_size + sizeof(read_reply->_offset);
                read_reply->_offset = 0;

                const char *teds_payload = teds->get_payload();
                for (unsigned short i = 0; i < teds_size; i++)
                    reply[i + sizeof(TEDS_Read_Reply)] = teds_payload[i];

                IEEE1451_Dot5_TIM::get_instance()->send_msg(trans_id, reply, reply_size);
                delete[] reply;
            }else
            {
                TEDS_Read_Reply reply;
                reply._header._success = false;
                reply._header._length = sizeof(reply._offset);
                reply._offset = 0;

                IEEE1451_Dot5_TIM::get_instance()->send_msg(trans_id, (char *) &reply, sizeof(TEDS_Read_Reply));
            }

            break;
        }

        case COMMAND_CLASS_READ_TRANSDUCER_CHANNEL_DATA_SET_SEGMENT:
        {
            unsigned int offset = *((unsigned int *) buffer);
            read_data_set(trans_id, offset);
            break;
        }

        case COMMAND_CLASS_TRANSDUCER_CHANNEL_OPERATE:
            start();
            break;

        case COMMAND_CLASS_TRANSDUCER_CHANNEL_IDLE:
            stop();
            break;

        default:
            db<IEEE1451_Dot0_TIM>(INF) << "Received invalid message\n";
            break;
    }
}

//-------------------------------------------

IEEE1451_Dot0_TIM *IEEE1451_Dot0_TIM::_dot0 = 0;

IEEE1451_Dot0_TIM::IEEE1451_Dot0_TIM()
{
    _transducer = 0;
    IEEE1451_Dot5_TIM::get_instance();
    init_teds();
}

IEEE1451_Dot0_TIM::~IEEE1451_Dot0_TIM()
{
    if (_meta_array)
        delete _meta_array;
    if (_tim_utn_array)
        delete _tim_utn_array;
    if (_phy_array)
        delete _phy_array;

    if (_meta_teds)
        delete _meta_teds;
    if (_tim_utn_teds)
        delete _tim_utn_teds;
    if (_phy_teds)
        delete _phy_teds;
}

IEEE1451_Dot0_TIM *IEEE1451_Dot0_TIM::get_instance()
{
    if (!_dot0)
        _dot0 = new IEEE1451_Dot0_TIM();
    return _dot0;
}

void IEEE1451_Dot0_TIM::init_teds()
{
    _meta_array = new char[40];
    _meta_array[0] = 0x00; _meta_array[1] = 0x00; _meta_array[2] = 0x00; _meta_array[3] = 0x24; _meta_array[4] = TEDS_META_TEDS_ID; _meta_array[5] = 0x04; _meta_array[6] = 0x00; _meta_array[7] = TEDS_META; _meta_array[8] = 0x01; _meta_array[9] = 0x01; _meta_array[10] = TEDS_META_UUID; _meta_array[11] = 0x0a; _meta_array[12] = 0x00; _meta_array[13] = 0x00; _meta_array[14] = 0x00; _meta_array[15] = 0x00; _meta_array[16] = 0x00; _meta_array[17] = 0x00; _meta_array[18] = 0x00; _meta_array[19] = 0x00; _meta_array[20] = 0x00; _meta_array[21] = 0x00; _meta_array[22] = TEDS_META_O_HOLD_OFF; _meta_array[23] = 0x04; _meta_array[24] = 0x41; _meta_array[25] = 0x20; _meta_array[26] = 0x00; _meta_array[27] = 0x00; _meta_array[28] = TEDS_META_TEST_TIME; _meta_array[29] = 0x04; _meta_array[30] = 0x00; _meta_array[31] = 0x00; _meta_array[32] = 0x00; _meta_array[33] = 0x00; _meta_array[34] = TEDS_META_MAX_CHAN; _meta_array[35] = 0x02; _meta_array[36] = 0x00; _meta_array[37] = 0x01; _meta_array[38] = 0xfe; _meta_array[39] = 0xb6; //checksum errado
    _meta_teds = new IEEE1451_TEDS_TIM(_meta_array, 40);

    _tim_utn_array = new char[26];
    _tim_utn_array[0] = 0x00; _tim_utn_array[1] = 0x00; _tim_utn_array[2] = 0x00; _tim_utn_array[3] = 0x16; _tim_utn_array[4] = TEDS_USER_TRANSDUCER_NAME_TEDS_ID; _tim_utn_array[5] = 0x04; _tim_utn_array[6] = 0x00; _tim_utn_array[7] = TEDS_USER_TRANSDUCER_NAME; _tim_utn_array[8] = 0x01; _tim_utn_array[9] = 0x01; _tim_utn_array[10] = TEDS_USER_TRANSDUCER_NAME_FORMAT; _tim_utn_array[11] = 0x01; _tim_utn_array[12] = 0x00; _tim_utn_array[13] = TEDS_USER_TRANSDUCER_NAME_TC_NAME; _tim_utn_array[14] = 0x09; _tim_utn_array[15] = 0x45; _tim_utn_array[16] = 0x50; _tim_utn_array[17] = 0x4f; _tim_utn_array[18] = 0x53; _tim_utn_array[19] = 0x2f; _tim_utn_array[20] = 0x4d; _tim_utn_array[21] = 0x6f; _tim_utn_array[22] = 0x74; _tim_utn_array[23] = 0x65; _tim_utn_array[24] = 0xff; _tim_utn_array[25] = 0xff; //checksum errado
    _tim_utn_teds = new IEEE1451_TEDS_TIM(_tim_utn_array, 26);

    _phy_array = new char[25];
    _phy_array[0] = 0x00; _phy_array[1] = 0x00; _phy_array[2] = 0x00; _phy_array[3] = 0x15; _phy_array[4] = TEDS_PHY_TEDS_ID; _phy_array[5] = 0x04; _phy_array[6] = 0x05; _phy_array[7] = TEDS_PHY; _phy_array[8] = 0x01; _phy_array[9] = 0x01; _phy_array[10] = TEDS_PHY_RADIO; _phy_array[11] = 0x01; _phy_array[12] = 0xff; _phy_array[13] = TEDS_PHY_MAX_C_DEV; _phy_array[14] = 0x02; _phy_array[15] = 0x00; _phy_array[16] = 0x01; _phy_array[17] = TEDS_PHY_BATTERY; _phy_array[18] = 0x01; _phy_array[19] = 0x01; _phy_array[20] = TEDS_PHY_RECONNECT; _phy_array[21] = 0x01; _phy_array[22] = 0x01; _phy_array[23] = 0xfe; _phy_array[24] = 0x74; //checksum errado
    _phy_teds = new IEEE1451_TEDS_TIM(_phy_array, 25);
}

IEEE1451_TEDS_TIM *IEEE1451_Dot0_TIM::get_teds(char id)
{
    if (id == 0x01)
        return _meta_teds;
    else if (id == 0x0c)
        return _tim_utn_teds;
    else if (id == 0x0d)
        return _phy_teds;
    return 0;
}

void IEEE1451_Dot0_TIM::receive_msg(unsigned short trans_id, const char *message, unsigned int size)
{
    Command *cmd = (Command *) message;
    const char *buffer = message + sizeof(Command);

    if (cmd->_channel_number == ADDRESS_CLASS_TIM)
    {
        switch (cmd->_command)
        {
            /*case COMMAND_CLASS_QUERY_TEDS:
             case COMMAND_CLASS_UPDATE_TEDS:
             {
                 TEDS *teds = get_teds(buffer[0]);
                 TEDS_Query_Reply reply;
                 reply._header._length = 12;
                 if (teds)
                 {
                     reply._maxSize = 0;
                     reply._checksum = *((unsigned short *) teds->get_payload());
                     reply._size = teds->get_size();
                     reply._status = teds->get_status();
                     reply._atributes = teds->get_atributes();
                     reply._header._success = true;
                 }else
                 {
                     reply._maxSize = 0;
                     reply._checksum = 0;
                     reply._size = 0;
                     reply._status = 0;
                     reply._atributes = 0x40;
                     reply._header._success = false;
                 }

                 _dot5->send_msg(trans_id, (char *) &reply, sizeof(reply));
                 break;
             }*/

            case COMMAND_CLASS_READ_TEDS_SEGMENT:
            {
                IEEE1451_TEDS_TIM *teds = get_teds(buffer[0]);
                if (teds)
                {
                    unsigned int teds_size = teds->get_size();
                    unsigned int reply_size = sizeof(TEDS_Read_Reply) + teds_size;
                    char *reply = new char[reply_size];

                    TEDS_Read_Reply *read_reply = (TEDS_Read_Reply *) reply;
                    read_reply->_header._success = true;
                    read_reply->_header._length = teds_size + sizeof(read_reply->_offset);
                    read_reply->_offset = 0;

                    const char *teds_payload = teds->get_payload();
                    for (unsigned short i = 0; i < teds_size; i++)
                        reply[i + sizeof(TEDS_Read_Reply)] = teds_payload[i];

                    IEEE1451_Dot5_TIM::get_instance()->send_msg(trans_id, reply, reply_size);
                    delete[] reply;
                }else
                {
                    TEDS_Read_Reply reply;
                    reply._header._success = false;
                    reply._header._length = sizeof(reply._offset);
                    reply._offset = 0;

                    IEEE1451_Dot5_TIM::get_instance()->send_msg(trans_id, (char *) &reply, sizeof(TEDS_Read_Reply));
                }
                break;
            }

            default:
                db<IEEE1451_Dot0_TIM>(INF) << "Received invalid message\n";
                break;
        }
    } else if (((cmd->_channel_number >= ADDRESS_CLASS_TRANSDUCER_CHANNEL_FIRST) && (cmd->_channel_number <= ADDRESS_CLASS_TRANSDUCER_CHANNEL_LAST)) ||
                (cmd->_channel_number == ADDRESS_CLASS_GLOBAL))
    {
        _transducer->receive_msg(trans_id, message, size);

    } else if ((cmd->_channel_number >= ADDRESS_CLASS_GROUP_FIRST) && (cmd->_channel_number <= ADDRESS_CLASS_GROUP_LAST))
    {
        //group
    }
}

//-------------------------------------------

IEEE1451_Dot5_TIM *IEEE1451_Dot5_TIM::_dot5 = 0;

IEEE1451_Dot5_TIM::IEEE1451_Dot5_TIM() : _tcp(IP::instance()), _ncap_address((unsigned long) 0x0a00000b)
{
    _socket = 0;
    _connected = false;
}

IEEE1451_Dot5_TIM *IEEE1451_Dot5_TIM::get_instance()
{
    if (!_dot5)
        _dot5 = new IEEE1451_Dot5_TIM();
    return _dot5;
}

void IEEE1451_Dot5_TIM::connect()
{
    //TODO: Voltar a receber dados da rede!

    db<IEEE1451_Dot5_TIM>(INF) << "Connecting...\n";
    if (_socket)
        delete _socket; //TODO
    _socket = new My_Client_Socket(&_tcp, _ncap_address);

    while (!_connected)
        Alarm::delay(1000);
}

void IEEE1451_Dot5_TIM::disconnect()
{
    //TODO: Parar de receber dados da rede!

    db<IEEE1451_Dot5_TIM>(INF) << "Disconnecting...\n";
    if (_socket)
        _socket->close();

    while (_connected)
        Alarm::delay(1000);

    delete _socket;
    _socket = 0;
}

void IEEE1451_Dot5_TIM::send_msg(unsigned short trans_id, const char *message, unsigned int length)
{
    db<IEEE1451_Dot5_TIM>(INF) << "Sending message (trans_id=" << trans_id << ")...\n";

    if (!_socket)
    {
        db<IEEE1451_Dot5_TIM>(INF) << "Failed to send message (trans_id=" << trans_id << ")\n";
        return;
    }

    unsigned int data_length = sizeof(IEEE1451_Packet) + length;
    char *data = new char[data_length];

    IEEE1451_Packet *out = (IEEE1451_Packet *) data;
    char *msg = data + sizeof(IEEE1451_Packet);

    out->_trans_id = trans_id;
    out->_length = length;
    memcpy(msg, message, length);

    _socket->send(data, data_length);
    delete data;
}

void IEEE1451_Dot5_TIM::My_Client_Socket::connected()
{
    db<IEEE1451_Dot5_TIM::My_Client_Socket>(INF) << "Client socket connected\n";
    IEEE1451_Dot5_TIM::get_instance()->_connected = true;
}

void IEEE1451_Dot5_TIM::My_Client_Socket::closed()
{
    db<IEEE1451_Dot5_TIM::My_Client_Socket>(INF) << "Client socket closed\n";
    IEEE1451_Dot5_TIM::get_instance()->_connected = false;
}

void IEEE1451_Dot5_TIM::My_Client_Socket::received(const char *data, u16 size)
{
    db<IEEE1451_Dot5_TIM::My_Client_Socket>(INF) << "Client socket received message\n";

    if (!(remote().ip() == IEEE1451_Dot5_TIM::get_instance()->_ncap_address))
    {
        db<IEEE1451_Dot5_TIM::My_Client_Socket>(INF) << "Message source is not NCAP Address!\n";
        return;
    }

    if (!IEEE1451_Dot5_TIM::get_instance()->_connected)
    {
        db<IEEE1451_Dot5_TIM::My_Client_Socket>(INF) << "Dot5 not connected!\n";
        return;
    }

    IEEE1451_Packet *in = (IEEE1451_Packet *) data;
    const char *msg = data + sizeof(IEEE1451_Packet);

    if (in->_length > 0)
        IEEE1451_Dot0_TIM::get_instance()->receive_msg(in->_trans_id, msg, in->_length);
}

__END_SYS
