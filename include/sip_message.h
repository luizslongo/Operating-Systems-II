#ifndef __sip_message_h
#define __sip_message_h

#include <sip_defs.h>
#include <sip_header.h>
#include <sip_body.h>
#include <utility/list.h>
#include <utility/malloc.h>

__BEGIN_SYS

class SIP_Manager;

class SIP_Message
{
    friend class SIP_Manager;

public:
    SIP_Message() : _body(0), _can_delete(true), _link(this) {};
    //SIP_Message(SIP_Message &msg);
    virtual ~SIP_Message();

    static SIP_Message *decode_msg(const char *sip_msg);

    static SIP_Message_Type get_msg_type(const char *sip_msg);
    static const char *get_msg_type(const SIP_Message_Type type);
    virtual SIP_Message_Type get_msg_type() = 0;

    bool parse(const char *sip_msg);
    virtual bool parse_start_line(const char *sip_msg) = 0;
    bool parse_header(const char *sip_msg);
    bool parse_body(const char *sip_msg);

    bool encode(char *sip_msg);
    virtual bool encode_start_line(char *sip_msg) = 0;
    bool encode_header(char *sip_msg, char *body_msg);
    bool encode_body(char *sip_msg);

    void add_header(SIP_Header *header);
    void add_headers(Simple_List<SIP_Header> &headers);
    SIP_Header *get_header(int header_type, int pos = 0);
    int get_num_header(int header_type);

    void set_body(SIP_Body *body) { _body = body; }
    SIP_Body *get_body() { return _body; }

    static SIP_Transport_Type get_transport_type(const char *type);
    static const char *get_transport_type(const SIP_Transport_Type type);

    void set_can_delete(bool can_delete) { _can_delete = can_delete; }
    bool get_can_delete() { return _can_delete; }

private:
    Simple_List<SIP_Header> _headers;
    SIP_Body *_body;
    bool _can_delete;

    Simple_List<SIP_Message>::Element _link;
};


class SIP_Request_Line
{
public:
    SIP_Request_Line() : _method(SIP_MESSAGE_TYPE_INVALID), _request_uri(0), _sip_version(0) {}
    //SIP_Request_Line(const SIP_Request_Line &request);
    //SIP_Request_Line(const SIP_Message_Type msg_type, const char *request_uri, const char *sip_version = SIP_VERSION);
    ~SIP_Request_Line();

    bool parse(const SIP_Message_Type msg_type, char *sip_msg);
    bool encode(char *sipMsg);

    void set_request_line(const SIP_Message_Type msg_type, const char *request_uri, const char *sip_version);

    SIP_Message_Type get_method() const { return _method; }
    const char *get_request_uri() const { return _request_uri; }
    //const char *get_sip_version() const { return _sip_version; }

private:
    SIP_Message_Type _method;
    char *_request_uri;
    char *_sip_version;
};


class SIP_Request : public SIP_Message
{
public:
    SIP_Request() {}
    //SIP_Request(SIP_Request &request) : SIP_Message(request), _request_line(request._request_line) {}
    //SIP_Request(SIP_Message_Type msg_type, char *request_uri, char *sip_version = SIP_VERSION);
    virtual ~SIP_Request() {}

    bool parse_start_line(const char *sip_msg);
    bool encode_start_line(char *sip_msg) { return _request_line.encode(sip_msg); }

    void set_request_line(const SIP_Message_Type msg_type, const char *request_uri, const char *sip_version = SIP_VERSION)
    { _request_line.set_request_line(msg_type, request_uri, sip_version); }
    const SIP_Request_Line *get_request_line() { return &_request_line; }

private:
    SIP_Request_Line _request_line;
};


class SIP_Request_Ack : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_ACK; }
};


class SIP_Request_Bye : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_BYE; }
};


/*class SIP_Request_Cancel : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_CANCEL; }
};*/


class SIP_Request_Invite : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_INVITE; }
};


class SIP_Request_Message : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_MESSAGE; }
};


class SIP_Request_Notify : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_NOTIFY; }
};


/*class SIP_Request_Options : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_OPTIONS; }
};*/


/*class SIP_Request_Register : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_REGISTER; }
};*/


class SIP_Request_Subscribe : public SIP_Request
{
public:
    SIP_Message_Type get_msg_type() { return SIP_REQUEST_SUBSCRIBE; }
};


class SIP_Status_Line
{
public:
    SIP_Status_Line() : _sip_version(0), _status_code(0), _reason_phrase(0) {};
    //SIP_Status_Line(const SIP_Status_Line &status);
    //SIP_Status_Line(const char *sip_version, int code, const char *reason);
    ~SIP_Status_Line();

    bool parse(const char *version, char *sip_msg);
    bool encode(char *sip_msg);

    void set_status_line(const char *sip_version, int status_code, const char *reason_phrase);

    const char *get_sip_version() const { return _sip_version; }
    int get_status_code() const { return _status_code; }
    //const char *get_reason_phrase() const { return _reason_phrase; }

private:
    char *_sip_version;
    int _status_code;
    char *_reason_phrase;
};


class SIP_Response : public SIP_Message
{
public:
    SIP_Response() {};
    //SIP_Response(SIP_Response &response) : SIP_Message(response), _status_line(response._status_line) {}
    SIP_Response(int status_code);
    ~SIP_Response() {};

    bool parse_start_line(const char *sipMsg);
    bool encode_start_line(char *sip_msg) { return _status_line.encode(sip_msg); }

    SIP_Message_Type get_msg_type() { return SIP_RESPONSE; }

    const SIP_Status_Line *get_status_line() { return &_status_line; }

private:
    SIP_Status_Line _status_line;

    static SIP_Status_Code _status_codes[];
};

__END_SYS

#endif
