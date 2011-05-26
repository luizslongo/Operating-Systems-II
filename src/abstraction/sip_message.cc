#include <sip_message.h>
#include <sip_header.h>

__BEGIN_SYS

/*SIP_Message::SIP_Message(SIP_Message &msg) : _link(this)
{
    _can_delete = true;

    Simple_List<SIP_Header>::Iterator it = _headers();
    while (it != msg._headers.end())
    {
        SIP_Header *header = it->object();
        it++;

        SIP_Header *header2 = SIP_Header::create_header(header->get_header_type(), header);
        add_header(header2);
    }

    if (msg._body)
        _body = SIP_Body::create_body(msg._body->get_body_type(), msg._body);
    else
        _body = 0;
}*/

SIP_Message::~SIP_Message()
{
    Simple_List<SIP_Header>::Iterator it = _headers.begin();
    while (it != _headers.end())
    {
        SIP_Header *header = it->object();
        _headers.remove(it++);
        delete header;
    }

    if (_body)
        delete _body;
}

SIP_Message *SIP_Message::decode_msg(const char *sip_msg)
{
    char *p_sip_msg = (char *) sip_msg;
    remove_lws(p_sip_msg);

    char type[30];
    match(p_sip_msg, " ", type);
    trim(type);
    SIP_Message_Type msg_type = get_msg_type(type);

    SIP_Message *msg = 0;

    switch (msg_type)
    {
        case SIP_REQUEST_ACK:        msg = new SIP_Request_Ack();       break;
        case SIP_REQUEST_BYE:        msg = new SIP_Request_Bye();       break;
        //case SIP_REQUEST_CANCEL:   msg = new SIP_Request_Cancel();    break;
        case SIP_REQUEST_INVITE:     msg = new SIP_Request_Invite();    break;
        case SIP_REQUEST_MESSAGE:    msg = new SIP_Request_Message();   break;
        case SIP_REQUEST_NOTIFY:     msg = new SIP_Request_Notify();    break;
        //case SIP_REQUEST_OPTIONS:  msg = new SIP_Request_Options();   break;
        //case SIP_REQUEST_REGISTER: msg = new SIP_Request_Register();  break;
        case SIP_REQUEST_SUBSCRIBE:  msg = new SIP_Request_Subscribe(); break;
        case SIP_RESPONSE:           msg = new SIP_Response();          break;
        default: db<SIP_Message>(WRN) << "SIP_Message::decode_msg -> Invalid " << type << " message type\n"; break;
    }

    if ((msg) && (!msg->parse(p_sip_msg)))
    {
        db<SIP_Message>(WRN) << "SIP_Message::decode_msg -> Failed to parse " << type << " message type\n";
        delete msg;
        msg = 0;
    }

    return msg;
}

SIP_Message_Type SIP_Message::get_msg_type(const char *sip_msg)
{
    SIP_Message_Type type = SIP_MESSAGE_TYPE_INVALID;

    if (!strcmp(sip_msg, "ACK"))
        type = SIP_REQUEST_ACK;

    else if (!strcmp(sip_msg, "BYE"))
        type = SIP_REQUEST_BYE;

    /*else if (!strcmp(sip_msg, "CANCEL"))
        type = SIP_REQUEST_CANCEL;*/

    else if (!strcmp(sip_msg, "INVITE"))
        type = SIP_REQUEST_INVITE;

    else if (!strcmp(sip_msg, "MESSAGE"))
        type = SIP_REQUEST_MESSAGE;

    else if (!strcmp(sip_msg, "NOTIFY"))
        type = SIP_REQUEST_NOTIFY;

    /*else if (!strcmp(sip_msg, "OPTIONS"))
        type = SIP_REQUEST_OPTIONS;

    else if (!strcmp(sip_msg, "REGISTER"))
        type = SIP_REQUEST_REGISTER;*/

    else if (!strcmp(sip_msg, "SUBSCRIBE"))
        type = SIP_REQUEST_SUBSCRIBE;

    else if (!strcmp(sip_msg, SIP_VERSION))
        type = SIP_RESPONSE;

    return type;
}

const char *SIP_Message::get_msg_type(const SIP_Message_Type type)
{
    const char *sipMsg = 0;

    switch (type)
    {
        case SIP_REQUEST_ACK:        sipMsg = "ACK";       break;
        case SIP_REQUEST_BYE:        sipMsg = "BYE";       break;
        //case SIP_REQUEST_CANCEL:   sipMsg = "CANCEL";    break;
        case SIP_REQUEST_INVITE:     sipMsg = "INVITE";    break;
        case SIP_REQUEST_MESSAGE:    sipMsg = "MESSAGE";   break;
        case SIP_REQUEST_NOTIFY:     sipMsg = "NOTIFY";    break;
        //case SIP_REQUEST_OPTIONS:  sipMsg = "OPTIONS";   break;
        //case SIP_REQUEST_REGISTER: sipMsg = "REGISTER";  break;
        case SIP_REQUEST_SUBSCRIBE:  sipMsg = "SUBSCRIBE"; break;
        case SIP_RESPONSE:           sipMsg = SIP_VERSION; break;
        default: break;
    }

    return sipMsg;
}

bool SIP_Message::parse(const char *sip_msg)
{
    if (!parse_start_line(sip_msg))
        return false;

    if (!parse_header(sip_msg))
        return false;

    if (!parse_body(sip_msg))
        return false;

    return true;
}

bool SIP_Message::parse_header(const char *sip_msg)
{
    char line[MAX_LINE];
    Simple_List<SIP_Header> header;
    while (true)
    {
        bool ret = get_line(sip_msg, line);
        if ((!ret) || (!strcmp(line, "")))
            return true;

        SIP_Header::decode_header(line, header);
        add_headers(header);
    }

    return true;
}

bool SIP_Message::parse_body(const char *sip_msg)
{
    SIP_Header_Content_Type *content_type = (SIP_Header_Content_Type *) get_header(SIP_HEADER_CONTENT_TYPE);
    if (content_type)
        _body = SIP_Body::decode_body(sip_msg, content_type->get_content_type());
    return true;
}

bool SIP_Message::encode(char *sip_msg)
{
    char body_msg[512];
    body_msg[0] = 0;

    if (!encode_start_line(sip_msg))
        return false;

    if (!encode_body(body_msg))
        return false;

    if (!encode_header(sip_msg, body_msg))
        return false;

    return true;
}

bool SIP_Message::encode_header(char *sip_msg, char *body_msg)
{
    if (_body)
    {
        SIP_Header_Content_Type *header_content_type = (SIP_Header_Content_Type *) get_header(SIP_HEADER_CONTENT_TYPE);
        if (!header_content_type)
        {
            header_content_type = new SIP_Header_Content_Type();
            add_header(header_content_type);
        }
        header_content_type->set_content_type(_body->get_body_type());
    }

    SIP_Header_Content_Length *header_content_length = (SIP_Header_Content_Length *) get_header(SIP_HEADER_CONTENT_LENGTH);
    if (!header_content_length)
    {
        header_content_length = new SIP_Header_Content_Length();
        add_header(header_content_length);
    }
    header_content_length->set_number(strlen(body_msg));

    char *p_sip_msg;
    Simple_List<SIP_Header>::Iterator it = _headers.begin();
    while (it != _headers.end())
    {
        SIP_Header *header = it->object();
        it++;

        p_sip_msg = sip_msg + strlen(sip_msg);

        if (header)
            header->encode(p_sip_msg);
    }

    strcat(sip_msg, "\r\n");
    strcat(sip_msg, body_msg);
    return true;
}

bool SIP_Message::encode_body(char *sip_msg)
{
    if (_body)
        _body->encode(sip_msg);
    return true;
}

void SIP_Message::add_header(SIP_Header *header)
{
    if (header)
        _headers.insert(&header->_link);
}

void SIP_Message::add_headers(Simple_List<SIP_Header> &headers)
{
    Simple_List<SIP_Header>::Iterator it = headers.begin();
    while (it != headers.end())
    {
        SIP_Header *header = it->object();
        it++;
        headers.remove_head();
        _headers.insert(&header->_link);
    }
}

SIP_Header *SIP_Message::get_header(int header_type, int pos)
{
    int count = -1;
    Simple_List<SIP_Header>::Iterator it = _headers.begin();
    while (it != _headers.end())
    {
        SIP_Header *header = it->object();
        it++;

        if (header->get_header_type() == header_type)
        {
            count++;
            if (count == pos)
                return header;
        }
    }
    return 0;
}

int SIP_Message::get_num_header(int header_type)
{
    int count = 0;
    Simple_List<SIP_Header>::Iterator it = _headers.begin();
    while (it != _headers.end())
    {
        SIP_Header *header = it->object();
        it++;

        if (header->get_header_type() == header_type)
            count++;
    }
    return count;
}

SIP_Transport_Type SIP_Message::get_transport_type(const char *type)
{
    SIP_Transport_Type transport = SIP_TRANSPORT_TYPE_INVALID;

    if (!strcmp(type, "UDP"))
        transport = SIP_TRANSPORT_UDP;

    else if (!strcmp(type, "TCP"))
        transport = SIP_TRANSPORT_TCP;

    return transport;
}

const char *SIP_Message::get_transport_type(const SIP_Transport_Type type)
{
    const char *transport = 0;

    switch (type)
    {
        case SIP_TRANSPORT_UDP: transport = "UDP"; break;
        case SIP_TRANSPORT_TCP: transport = "TCP"; break;
        default: break;
    }

    return transport;
}

//-------------------------------------------

/*SIP_Request_Line::SIP_Request_Line(const SIP_Request_Line &request)
{
    _method = request._method;
    _request_uri = create_string(request._request_uri);
    _sip_version = create_string(request._sip_version);
}*/

/*SIP_Request_Line::SIP_Request_Line(const SIP_Message_Type msg_type, const char *request_uri, const char *sip_version)
{
    _method = msg_type;
    _request_uri = create_string(request_uri);
    _sip_version = create_string(sip_version);
}*/

SIP_Request_Line::~SIP_Request_Line()
{
    if (_request_uri)
        delete _request_uri;

    if (_sip_version)
        delete _sip_version;
}

bool SIP_Request_Line::parse(const SIP_Message_Type msg_type, char *sip_msg)
{
    _method = msg_type;

    char result[255];
    skip(sip_msg, " \t");
    match(sip_msg, " ", result);
    _request_uri = create_string(result);
    if (!_request_uri)
        return false;

    trim(sip_msg);
    _sip_version = create_string(sip_msg);
    if (!_sip_version)
        return false;

    return true;
}

bool SIP_Request_Line::encode(char *sip_msg)
{
    strcat(sip_msg, SIP_Message::get_msg_type(_method));
    strcat(sip_msg, " ");
    strcat(sip_msg, _request_uri);
    strcat(sip_msg, " ");
    strcat(sip_msg, _sip_version);
    strcat(sip_msg, "\r\n");
    return true;
}

void SIP_Request_Line::set_request_line(const SIP_Message_Type msg_type, const char *request_uri, const char *sip_version)
{
    _method = msg_type;

    if (_request_uri)
        delete _request_uri;
    _request_uri = create_string(request_uri);

    if (_sip_version)
        delete _sip_version;
    _sip_version = create_string(sip_version);
}

//-------------------------------------------

/*SIP_Request::SIP_Request(SIP_Message_Type msg_type, char *request_uri, char *sip_version)
{
    set_request_line(msg_type, request_uri, sip_version);
}*/

bool SIP_Request::parse_start_line(const char *sip_msg)
{
    char line[MAX_LINE];
    get_line(sip_msg, line);

    if (!_request_line.parse(get_msg_type(), line))
    {
        db<SIP_Request>(WRN) << "SIP_Request::parse_start_line -> parse failed\n";
        return false;
    }

    return true;
}

//-------------------------------------------

SIP_Status_Code SIP_Response::_status_codes[] =
{
    //Informational
    { 100, "Trying" },
    { 180, "Ringing" },
    { 181, "Call is Being Forwarded" },
    { 182, "Queued" },
    { 183, "Session Progress" },

    //Success
    { 200, "OK" },
    { 202, "Accepted" },

    //Redirection
    { 300, "Multiple Choices" },
    { 301, "Moved Permanently" },
    { 302, "Moved Temporarily" },
    //{ 303, "See Other" },
    { 305, "Use Proxy" },
    { 380, "Alternative Service" },

    //Client-Error
    { 400, "Bad Request" },
    { 401, "Unauthorized" },
    { 402, "Payment Required" },
    { 403, "Forbidden" },
    { 404, "Not Found" },
    { 405, "Method Not Allowed" },
    { 406, "Not Acceptable" },
    { 407, "Proxy Authentication Required" },
    { 408, "Request Timeout" },
    { 409, "Conflict" },
    { 410, "Gone" },
    //{ 411, "Length Required" },
    { 413, "Request Entity Too Large" },
    { 414, "Request-URI Too Large" },
    { 415, "Unsupported Media Type" },
    { 420, "Bad Extension" },
    { 480, "Temporarily Unavailable" },
    { 481, "Call Leg/Transaction Does Not Exist" },
    { 482, "Loop Detected" },
    { 483, "Too Many Hops" },
    { 484, "Address Incomplete" },
    { 485, "Ambiguous" },
    { 486, "Busy Here" },
    { 487, "Request Terminated" },
    { 488, "Not Acceptable Here" },
    { 489, "Bad Event" },

    //Server-Error
    { 500, "Server Internal Error" },
    { 501, "Not Implemented" },
    { 502, "Bad Gateway" },
    { 503, "Service Unavailable" },
    { 504, "Server Timed-out" },
    { 505, "Version Not Supported" },
    { 513, "Message Too Large" },

    //Global-Failure
    { 600, "Busy Everywhere" },
    { 603, "Decline" },
    { 604, "Does Not Exist Anywhere" },
    { 606, "Not Acceptable" },
    { 0, "Unknown" },
};

/*SIP_Status_Line::SIP_Status_Line(const SIP_Status_Line &status)
{
    _sip_version = create_string(status._sip_version);
    _status_code = status._status_code;
    _reason_phrase = create_string(status._reason_phrase);
}*/

/*SIP_Status_Line::SIP_Status_Line(const char *sip_version, int code, const char *reason)
{
    _sip_version = create_string(sip_version);
    _status_code = code;
    _reason_phrase = create_string(reason);
}*/

SIP_Status_Line::~SIP_Status_Line()
{
    if (_sip_version)
        delete _sip_version;

    if (_reason_phrase)
        delete _reason_phrase;
}

bool SIP_Status_Line::parse(const char *version, char *sip_msg)
{
    _sip_version = create_string(version);
    if (!_sip_version)
        return false;

    char result[255];
    skip(sip_msg, " \t");
    match(sip_msg, " ", result);
    int size = strlen(result);
    if (size == 0)
        return false;
    _status_code = (int) atol(result);

    trim(sip_msg);
    _reason_phrase = create_string(sip_msg);
    if (!_reason_phrase)
        return false;

    return true;
}

bool SIP_Status_Line::encode(char *sip_msg)
{
    strcat(sip_msg, _sip_version);
    strcat(sip_msg, " ");

    char value[11];
    itoa(_status_code, value);
    strcat(sip_msg, value);
    strcat(sip_msg, " ");

    strcat(sip_msg, _reason_phrase);
    strcat(sip_msg, "\r\n");
    return true;
}

void SIP_Status_Line::set_status_line(const char *sip_version, int status_code, const char *reason_phrase)
{
    if (_sip_version)
        delete _sip_version;
    _sip_version = create_string(sip_version);

    _status_code = status_code;

    if (_reason_phrase)
        delete _reason_phrase;
    _reason_phrase = create_string(reason_phrase);
}

//-------------------------------------------

SIP_Response::SIP_Response(int status_code)
{
    int i = 0;
    while (_status_codes[i]._code != 0)
    {
        if (_status_codes[i]._code == status_code)
        {
            _status_line.set_status_line(SIP_VERSION, status_code, _status_codes[i]._reason_phrase);
            break;
        }
        i++;
    }
}

bool SIP_Response::parse_start_line(const char *sip_msg)
{
    char line[MAX_LINE];
    get_line(sip_msg, line);

    if (!_status_line.parse(SIP_VERSION, line))
    {
        db<SIP_Response>(WRN) << "SIP_Response::parse_start_line -> parse failed\n";
        return false;
    }

    return true;
}

__END_SYS
