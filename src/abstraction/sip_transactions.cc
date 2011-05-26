#include <sip_transactions.h>
#include <sip_user_agent.h>

__BEGIN_SYS

SIP_Transaction::SIP_Transaction(SIP_User_Agent *ua) : _ua(ua), _link(this)
{
    _transport = SIP_Manager::get_instance()->get_transport();
}

void SIP_Transaction::clear()
{
    _ua->remove_transaction(this);
}

//-------------------------------------------

SIP_Transaction_Client_Invite::SIP_Transaction_Client_Invite(SIP_User_Agent *ua) : SIP_Transaction(ua)
{
    _state = sttIdle;
    _request_msg = 0;
}

SIP_Transaction_Client_Invite::~SIP_Transaction_Client_Invite()
{
    _ua->stop_timer(SIP_TIMER_A);
    _ua->stop_timer(SIP_TIMER_B);
    _ua->stop_timer(SIP_TIMER_D);

    if (_request_msg)
        delete _request_msg;
}

void SIP_Transaction_Client_Invite::send_ack(SIP_Response *msg)
{
    const char *request_uri = _request_msg->get_request_line()->get_request_uri();
    SIP_Header_Via *via = (SIP_Header_Via *) _request_msg->get_header(SIP_HEADER_VIA);
    SIP_Header_From *from = (SIP_Header_From *) _request_msg->get_header(SIP_HEADER_FROM);
    SIP_Header_Call_ID *callId = (SIP_Header_Call_ID *) _request_msg->get_header(SIP_HEADER_CALLID);
    SIP_Header_CSeq *cseq = (SIP_Header_CSeq *) _request_msg->get_header(SIP_HEADER_CSEQ);
    SIP_Header_To *to = (SIP_Header_To *) msg->get_header(SIP_HEADER_TO);

    if ((!request_uri) || (!via) || (!from) || (!callId) || (!cseq) || (!to))
        return;

    SIP_Request_Ack ack;
    ack.set_request_line(SIP_REQUEST_ACK, request_uri, SIP_VERSION);
    ack.add_header(new SIP_Header_Via(*via));
    ack.add_header(new SIP_Header_To(*to));
    ack.add_header(new SIP_Header_From(*from));
    ack.add_header(new SIP_Header_Call_ID(*callId));

    SIP_Header_Max_Forwards *headerMaxForwards = new SIP_Header_Max_Forwards();
    headerMaxForwards->set_number(70);
    ack.add_header(headerMaxForwards);

    SIP_Header_CSeq *headerCSeq = new SIP_Header_CSeq();
    headerCSeq->set_cseq(SIP_REQUEST_ACK, cseq->get_sequence());
    ack.add_header(headerCSeq);

    int routeNum = _request_msg->get_num_header(SIP_HEADER_ROUTE);
    for (int i = 0; i < routeNum; i++)
    {
        SIP_Header_Route *header = (SIP_Header_Route *) _request_msg->get_header(SIP_HEADER_ROUTE, i);
        ack.add_header(new SIP_Header_Route(*header));
    }

    _transport->send_message(&ack);
}

void SIP_Transaction_Client_Invite::send_invite(SIP_Request_Invite *msg)
{
    switch (_state)
    {
        case sttIdle:
            _state = sttCalling;
            _request_msg = msg; //_request_msg = new SIP_Request_Invite(*msg);
            msg->set_can_delete(false);
            _transport->send_message(_request_msg);
            _ua->setTimerValue(SIP_TIMER_A, SIP_TIMER_1);
            _ua->start_timer(SIP_TIMER_A, this);
            _ua->setTimerValue(SIP_TIMER_B, SIP_TIMER_1 * 64);
            _ua->start_timer(SIP_TIMER_B, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::receive_1xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttCalling:
            _state = sttProceeding;
            _ua->stop_timer(SIP_TIMER_A);
            _ua->stop_timer(SIP_TIMER_B);
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        case sttProceeding:
            //_state = sttProceeding;
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::receive_2xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttCalling:
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_A);
            _ua->stop_timer(SIP_TIMER_B);
            _ua->_uac.receive_msg(_request_msg, msg, this);
            clear();
            break;

        case sttProceeding:
            //_state = sttTerminated;
            _ua->_uac.receive_msg(_request_msg, msg, this);
            clear();
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::receive_3xx_6xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttCalling:
            _state = sttCompleted;
            send_ack(msg);
            _ua->setTimerValue(SIP_TIMER_D, SIP_TIMER_32s);
            _ua->start_timer(SIP_TIMER_D, this);
            _ua->stop_timer(SIP_TIMER_A);
            _ua->stop_timer(SIP_TIMER_B);
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        case sttProceeding:
            _state = sttCompleted;
            send_ack(msg);
            _ua->setTimerValue(SIP_TIMER_D, SIP_TIMER_32s);
            _ua->start_timer(SIP_TIMER_D, this);
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        case sttCompleted:
            //_state = sttCompleted;
            send_ack(msg);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::transport_error()
{
    switch (_state)
    {
        case sttCalling:
        {
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_A);
            _ua->stop_timer(SIP_TIMER_B);

            SIP_Response response(503);
            _ua->_uac.receive_msg(_request_msg, &response, this);
            clear();
            break;
        }

        case sttCompleted:
        {
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_D);

            SIP_Response response(503);
            _ua->_uac.receive_msg(_request_msg, &response, this);
            clear();
            break;
        }

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::timer_A_Expired()
{
    switch (_state)
    {
        case sttCalling:
            //_state = sttCalling;
            _ua->setTimerValue(SIP_TIMER_A, _ua->getTimerValue(SIP_TIMER_A) * 2);
            _ua->start_timer(SIP_TIMER_A, this);
            _transport->send_message(_request_msg);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::timer_B_Expired()
{
    switch (_state)
    {
        case sttCalling:
        {
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_A);

            SIP_Response response(408);
            _ua->_uac.receive_msg(_request_msg, &response, this);
            clear();
            break;
        }

        default:
            break;
    }
}

void SIP_Transaction_Client_Invite::timer_D_Expired()
{
    switch (_state)
    {
        case sttCompleted:
            //_state = sttTerminated;
            clear();
            break;

        default:
            break;
    }
}

SIP_Transaction *SIP_Transaction_Client_Invite::matching_transaction(SIP_Message *msg)
{
    if ((!msg) || (msg->get_msg_type() != SIP_RESPONSE))
        return 0;

    SIP_Response *response = (SIP_Response *) msg;

    SIP_Header_Via *via_response = (SIP_Header_Via *) response->get_header(SIP_HEADER_VIA);
    SIP_Header_CSeq *cseq_response = (SIP_Header_CSeq *) response->get_header(SIP_HEADER_CSEQ);
    if ((!via_response) || (!cseq_response))
        return 0;

    const char *branch_response = via_response->get_branch();
    SIP_Message_Type msg_type_response = cseq_response->get_method();
    //unsigned int sequence_response = cseq_response->get_sequence();

    if (!branch_response)
        return 0;

    if (_request_msg)
    {
        SIP_Header_Via *via = (SIP_Header_Via *) _request_msg->get_header(SIP_HEADER_VIA);
        SIP_Header_CSeq *cseq = (SIP_Header_CSeq *) _request_msg->get_header(SIP_HEADER_CSEQ);

        if ((via) && (cseq))
        {
            const char *branch = via->get_branch();
            SIP_Message_Type msg_type = cseq->get_method();
            //unsigned int sequence = cseq->get_sequence();

            if ((!strcmp(branch, branch_response)) && (msg_type == msg_type_response)) // && (sequence == sequence_response))
                return this;
        }
    }

    return 0;
}

//-------------------------------------------

SIP_Transaction_Client_Non_Invite::SIP_Transaction_Client_Non_Invite(SIP_User_Agent *ua) : SIP_Transaction(ua)
{
    _state = sttIdle;
    _request_msg = 0;
}

SIP_Transaction_Client_Non_Invite::~SIP_Transaction_Client_Non_Invite()
{
    _ua->stop_timer(SIP_TIMER_E);
    _ua->stop_timer(SIP_TIMER_F);
    _ua->stop_timer(SIP_TIMER_K);

    if (_request_msg)
        delete _request_msg;
}

/*void SIP_Transaction_Client_Non_Invite::copy_request(Sip_Request *msg)
{
    switch (msg->get_msg_type())
    {
        case SIP_REQUEST_BYE:			_request_msg = new SIP_Request_Bye(*((SIP_Request_Bye *) msg));             break;
        //case SIP_REQUEST_CANCEL:		_request_msg = new SIP_Request_Cancel(*((SIP_Request_Cancel *) msg));       break;
        case SIP_REQUEST_MESSAGE:		_request_msg = new SIP_Request_Message(*((SIP_Request_Message *) msg));     break;
        case SIP_REQUEST_NOTIFY:		_request_msg = new SIP_Request_Notify(*((SIP_Request_Notify *) msg));       break;
        //case SIP_REQUEST_OPTIONS:		_request_msg = new SIP_Request_Options(*((SIP_Request_Options *) msg));     break;
        //case SIP_REQUEST_REGISTER:	_request_msg = new SIP_Request_Register(*((SIP_Request_Register *) msg));   break;
        case SIP_REQUEST_SUBSCRIBE:		_request_msg = new SIP_Request_Subscribe(*((SIP_Request_Subscribe *) msg)); break;
        default:						break;
    }
}*/

void SIP_Transaction_Client_Non_Invite::send_request(SIP_Request *msg)
{
    switch (_state)
    {
        case sttIdle:
            _state = sttTrying;
            _request_msg = msg; //copy_request(msg);
            msg->set_can_delete(false);
            _transport->send_message(_request_msg);
            _ua->setTimerValue(SIP_TIMER_E, SIP_TIMER_1);
            _ua->start_timer(SIP_TIMER_E, this);
            _ua->setTimerValue(SIP_TIMER_F, SIP_TIMER_1 * 64);
            _ua->start_timer(SIP_TIMER_F, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::receive_1xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
            _state = sttProceeding;
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::receive_2xx_6xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
            _state = sttCompleted;
            _ua->setTimerValue(SIP_TIMER_K, SIP_TIMER_4);
            _ua->start_timer(SIP_TIMER_K, this);
            _ua->stop_timer(SIP_TIMER_E);
            _ua->stop_timer(SIP_TIMER_F);
            _ua->_uac.receive_msg(_request_msg, msg, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::transport_error()
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
        {
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_E);
            _ua->stop_timer(SIP_TIMER_F);

            SIP_Response response(503);
            _ua->_uac.receive_msg(_request_msg, &response, this);
            clear();
            break;
        }

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::timer_E_Expired()
{
    switch (_state)
    {
        case sttTrying:
        {
            //_state = sttTrying;
            int timerValue = ((_ua->getTimerValue(SIP_TIMER_E) * 2) < SIP_TIMER_2) ? _ua->getTimerValue(SIP_TIMER_E) * 2 : SIP_TIMER_2;
            _ua->setTimerValue(SIP_TIMER_E, timerValue);
            _ua->start_timer(SIP_TIMER_E, this);
            _transport->send_message(_request_msg);
            break;
        }

        case sttProceeding:
            //_state = sttProceeding;
            _ua->setTimerValue(SIP_TIMER_E, SIP_TIMER_2);
            _ua->start_timer(SIP_TIMER_E, this);
            _transport->send_message(_request_msg);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::timer_F_Expired()
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
        {
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_E);

            SIP_Response response(408);
            _ua->_uac.receive_msg(_request_msg, &response, this);
            clear();
            break;
        }

        default:
            break;
    }
}

void SIP_Transaction_Client_Non_Invite::timer_K_Expired()
{
    switch (_state)
    {
        case sttCompleted:
            //_state = sttTerminated;
            clear();
            break;

        default:
            break;
    }
}

SIP_Transaction *SIP_Transaction_Client_Non_Invite::matching_transaction(SIP_Message *msg)
{
    if ((!msg) || (msg->get_msg_type() != SIP_RESPONSE))
        return 0;

    SIP_Response *response = (SIP_Response *) msg;

    SIP_Header_Via *via_response = (SIP_Header_Via *) response->get_header(SIP_HEADER_VIA);
    SIP_Header_CSeq *cseq_response = (SIP_Header_CSeq *) response->get_header(SIP_HEADER_CSEQ);
    if ((!via_response) || (!cseq_response))
        return 0;

    const char *branch_response = via_response->get_branch();
    SIP_Message_Type msg_type_response = cseq_response->get_method();
    //unsigned int sequence_response = cseq_response->get_sequence();

    if (!branch_response)
        return 0;

    if (_request_msg)
    {
        SIP_Header_Via *via = (SIP_Header_Via *) _request_msg->get_header(SIP_HEADER_VIA);
        SIP_Header_CSeq *cseq = (SIP_Header_CSeq *) _request_msg->get_header(SIP_HEADER_CSEQ);

        if ((via) && (cseq))
        {
            const char *branch = via->get_branch();
            SIP_Message_Type msg_type = cseq->get_method();
            //unsigned int sequence = cseq->get_sequence();

            if ((!strcmp(branch, branch_response)) && (msg_type == msg_type_response)) // && (sequence == sequence_response))
                return this;
        }
    }

    return 0;
}

//-------------------------------------------

SIP_Transaction_Server_Invite::SIP_Transaction_Server_Invite(SIP_User_Agent *ua) : SIP_Transaction(ua)
{
    _state = sttIdle;
    _request_msg = 0;
    _last_response = 0;
}

SIP_Transaction_Server_Invite::~SIP_Transaction_Server_Invite()
{
    _ua->stop_timer(SIP_TIMER_G);
    _ua->stop_timer(SIP_TIMER_H);
    _ua->stop_timer(SIP_TIMER_I);

    if (_request_msg)
        delete _request_msg;

    if (_last_response)
        delete _last_response;
}

void SIP_Transaction_Server_Invite::receive_invite(SIP_Request_Invite *msg)
{
    switch (_state)
    {
        case sttIdle:
            _state = sttProceeding;
            _request_msg = msg; //_request_msg = new SIP_Request_Invite(*msg);
            msg->set_can_delete(false);
            _ua->_uas.receive_msg(msg, this);
            break;

        case sttProceeding:
            //_state = sttProceeding;
            if (_last_response)
                _transport->send_message(_last_response);
            break;

        case sttCompleted:
            //_state = sttCompleted;
            if (_last_response)
                _transport->send_message(_last_response);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::receive_ack(SIP_Request_Ack *msg)
{
    switch (_state)
    {
        case sttCompleted:
            _state = sttConfirmed;
            _ua->setTimerValue(SIP_TIMER_I, SIP_TIMER_4);
            _ua->start_timer(SIP_TIMER_I, this);
            _ua->stop_timer(SIP_TIMER_G);
            _ua->stop_timer(SIP_TIMER_H);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::send_1xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttProceeding:
            //_state = sttProceeding;
            if (_last_response)
                delete _last_response;
            _last_response = msg; //_last_response = new SIP_Response(*msg);
            msg->set_can_delete(false);
            _transport->send_message(_last_response);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::send_2xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttProceeding:
            //_state = sttTerminated;
            _transport->send_message(msg);
            clear();
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::send_3xx_6xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttProceeding:
            _state = sttCompleted;
            if (_last_response)
                delete _last_response;
            _last_response = msg; //_last_response = new SIP_Response(*msg);
            msg->set_can_delete(false);
            _transport->send_message(_last_response);
            _ua->setTimerValue(SIP_TIMER_G, SIP_TIMER_1);
            _ua->start_timer(SIP_TIMER_G, this);
            _ua->setTimerValue(SIP_TIMER_H, SIP_TIMER_1 * 64);
            _ua->start_timer(SIP_TIMER_H, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::transport_error()
{
    switch (_state)
    {
        case sttProceeding:
            //_state = sttTerminated;
            //_ua->_uas.receive_msg(); //TODO: InformTU("Transport Error");
            clear();
            break;

        case sttCompleted:
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_G);
            _ua->stop_timer(SIP_TIMER_H);
            //_ua->_uas.receive_msg(); //TODO: InformTU("Transport Error");
            clear();
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::timer_G_Expired()
{
    switch (_state)
    {
        case sttCompleted:
        {
            //_state = sttCompleted;
            int timerValue = ((_ua->getTimerValue(SIP_TIMER_G) * 2) < SIP_TIMER_2) ? _ua->getTimerValue(SIP_TIMER_G) * 2 : SIP_TIMER_2;
            _ua->setTimerValue(SIP_TIMER_G, timerValue);
            _ua->start_timer(SIP_TIMER_G, this);
            if (_last_response)
                _transport->send_message(_last_response);
            break;
        }

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::timer_H_Expired()
{
    switch (_state)
    {
        case sttCompleted:
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_G);
            //_ua->_uas.receive_msg(); //TODO: InformTU("Timeout occurred - TimerH");
            clear();
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Invite::timer_I_Expired()
{
    switch (_state)
    {
        case sttConfirmed:
            //_state = sttTerminated;
            clear();
            break;

        default:
            break;
    }
}

SIP_Transaction *SIP_Transaction_Server_Invite::matching_transaction(SIP_Message *msg)
{
    if ((!msg) || ((msg->get_msg_type() != SIP_REQUEST_INVITE) && (msg->get_msg_type() != SIP_REQUEST_ACK)))
        return 0;

    SIP_Request *request = (SIP_Request *) msg;

    SIP_Header_Via *via_request = (SIP_Header_Via *) request->get_header(SIP_HEADER_VIA);
    if (!via_request)
        return 0;

    SIP_Message_Type method_request = request->get_request_line()->get_method();
    const char *branch_request = via_request->get_branch();
    const char *sent_by_request = via_request->get_sent_by();

    if ((!sent_by_request) || (method_request == SIP_MESSAGE_TYPE_INVALID))
        return 0;

    bool magic_cookie = false;
    if ((branch_request) && (start_with(branch_request, "z9hG4bK")))
        magic_cookie = true;

    if (magic_cookie)
    {
        SIP_Header_Via *via = (SIP_Header_Via *) _request_msg->get_header(SIP_HEADER_VIA);
        if (via)
        {
            SIP_Message_Type method = _request_msg->get_request_line()->get_method();
            const char *branch = via->get_branch();
            const char *sent_by = via->get_sent_by();

            if ((branch) && (sent_by) && (method != SIP_MESSAGE_TYPE_INVALID))
            {
                if ((!strcmp(branch, branch_request)) && (!strcmp(sent_by, sent_by_request)) &&
                    ((method == method_request) || ((method_request == SIP_REQUEST_ACK) && (method == SIP_REQUEST_INVITE))))
                {
                    return this;
                }
            }
        }
    }

    return 0;
}

//-------------------------------------------

SIP_Transaction_Server_Non_Invite::SIP_Transaction_Server_Non_Invite(SIP_User_Agent *ua) : SIP_Transaction(ua)
{
    _state = sttIdle;
    _request_msg = 0;
    _last_response = 0;
}

SIP_Transaction_Server_Non_Invite::~SIP_Transaction_Server_Non_Invite()
{
    _ua->stop_timer(SIP_TIMER_J);

    if (_request_msg)
        delete _request_msg;

    if (_last_response)
        delete _last_response;
}

/*void SIP_Transaction_Server_Non_Invite::copy_request(SIP_Request *msg)
{
    switch (msg->get_msg_type())
    {
        case SIP_REQUEST_BYE:			_request_msg = new SIP_Request_Bye(*((SIP_Request_Bye *) msg));             break;
        //case SIP_REQUEST_CANCEL:		_request_msg = new SIP_Request_Cancel(*((SIP_Request_Cancel *) msg));       break;
        case SIP_REQUEST_MESSAGE:		_request_msg = new SIP_Request_Message(*((SIP_Request_Message *) msg));     break;
        case SIP_REQUEST_NOTIFY:		_request_msg = new SIP_Request_Notify(*((SIP_Request_Notify *) msg));       break;
        //case SIP_REQUEST_OPTIONS:		_request_msg = new SIP_Request_Options(*((SIP_Request_Options *) msg));     break;
        //case SIP_REQUEST_REGISTER:	_request_msg = new SIP_Request_Register(*((SIP_Request_Register *) msg));   break;
        case SIP_REQUEST_SUBSCRIBE:		_request_msg = new SIP_Request_Subscribe(*((SIP_Request_Subscribe *) msg)); break;
        default:						break;
    }
}*/

void SIP_Transaction_Server_Non_Invite::receive_request(SIP_Request *msg)
{
    switch (_state)
    {
        case sttIdle:
            _state = sttTrying;
            _request_msg = msg; //copy_request(msg);
            msg->set_can_delete(false);
            _ua->_uas.receive_msg(msg, this);
            break;

        case sttProceeding:
            //_state = sttProceeding;
            if (_last_response)
                _transport->send_message(_last_response);
            break;

        case sttCompleted:
            //_state = sttCompleted;
            if (_last_response)
                _transport->send_message(_last_response);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Non_Invite::send_1xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
            _state = sttProceeding;
            if (_last_response)
                delete _last_response;
            _last_response = msg; //_last_response = new SIP_Response(*msg);
            msg->set_can_delete(false);
            _transport->send_message(_last_response);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Non_Invite::send_2xx_6xx(SIP_Response *msg)
{
    switch (_state)
    {
        case sttTrying:
        case sttProceeding:
            _state = sttCompleted;
            if (_last_response)
                delete _last_response;
            _last_response = msg; //_last_response = new SIP_Response(*msg);
            msg->set_can_delete(false);
            _transport->send_message(_last_response);
            _ua->setTimerValue(SIP_TIMER_J, SIP_TIMER_1 * 64);
            _ua->start_timer(SIP_TIMER_J, this);
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Non_Invite::transport_error()
{
    switch (_state)
    {
        case sttProceeding:
            //_state = sttTerminated;
            //_ua->_uas.receive_msg(); //TODO: InformTU("Transport Error");
            clear();
            break;

        case sttCompleted:
            //_state = sttTerminated;
            _ua->stop_timer(SIP_TIMER_J);
            //_ua->_uas.receive_msg(); //TODO: InformTU("Transport Error");
            clear();
            break;

        default:
            break;
    }
}

void SIP_Transaction_Server_Non_Invite::timer_J_Expired()
{
    switch (_state)
    {
        case sttCompleted:
            //_state = sttTerminated;
            clear();
            break;

        default:
            break;
    }
}

SIP_Transaction *SIP_Transaction_Server_Non_Invite::matching_transaction(SIP_Message *msg)
{
    if ((!msg) || (msg->get_msg_type() == SIP_RESPONSE) || (msg->get_msg_type() == SIP_REQUEST_INVITE) ||
        (msg->get_msg_type() == SIP_REQUEST_ACK))
    {
        return 0;
    }

    SIP_Request *request = (SIP_Request *) msg;

    SIP_Header_Via *via_request = (SIP_Header_Via *) request->get_header(SIP_HEADER_VIA);
    if (!via_request)
        return 0;

    SIP_Message_Type method_request = request->get_request_line()->get_method();
    const char *branch_request = via_request->get_branch();
    const char *sent_by_request = via_request->get_sent_by();

    if ((!sent_by_request) || (method_request == SIP_MESSAGE_TYPE_INVALID))
        return 0;

    bool magic_cookie = false;
    if ((branch_request) && (start_with(branch_request, "z9hG4bK")))
        magic_cookie = true;

    if (magic_cookie)
    {
        SIP_Header_Via *via = (SIP_Header_Via *) _request_msg->get_header(SIP_HEADER_VIA);
        if (via)
        {
            SIP_Message_Type method = _request_msg->get_request_line()->get_method();
            const char *branch = via->get_branch();
            const char *sent_by = via->get_sent_by();

            if ((branch) && (sent_by) && (method != SIP_MESSAGE_TYPE_INVALID))
            {
                if ((!strcmp(branch, branch_request)) && (!strcmp(sent_by, sent_by_request)) &&
                    ((method == method_request) || ((method_request == SIP_REQUEST_ACK) && (method == SIP_REQUEST_INVITE))))
                {
                    return this;
                }
            }
        }
    }

    return 0;
}

__END_SYS
