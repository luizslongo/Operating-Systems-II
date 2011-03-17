#ifndef __sip_body_h
#define __sip_body_h

#include <sip_defs.h>

__BEGIN_SYS


class SipBody
{
public:
	//SipBody() {};
	virtual ~SipBody() {}

	static SipBody *decodeBody(char *sipMsg, SipBodyType bodyType);
	static SipBody *createBody(SipBodyType type/*, const SipBody *copy = 0*/);

	virtual SipBodyType getBodyType() = 0;

	virtual bool parse(char *sipMsg) = 0;
	virtual bool encode(char *sipMsg) = 0;
};

//-------------------------------------------

class SipSdpBody : public SipBody
{
public:
	SipSdpBody() {};
	//SipSdpBody(SipSdpBody &body) {};
	~SipSdpBody() {};

	SipBodyType getBodyType() { return SIP_BODY_APPLICATION_SDP; };

	bool parse(char *sipMsg) { return true; };
	bool encode(char *sipMsg);
};

//-------------------------------------------

class SipPidfXmlBody : public SipBody
{
	SipPidfXmlBasicElement element;

public:
	SipPidfXmlBody() {};
	//SipPidfXmlBody(SipPidfXmlBody &body) { element = body.element; };
	~SipPidfXmlBody() {};

	SipBodyType getBodyType() { return SIP_BODY_APPLICATION_PIDF_XML; };

	bool parse(char *sipMsg) { return true; };
	bool encode(char *sipMsg);

	void setElement(SipPidfXmlBasicElement element) { this->element = element; };
};

//-------------------------------------------

class SipTextPlainBody : public SipBody
{
private:
	char *text;

public:
	SipTextPlainBody() : text(0) {};
	//SipTextPlainBody(SipTextPlainBody &body) { text = createString(body.text); };
	~SipTextPlainBody() { if (text) delete text; };

	SipBodyType getBodyType() { return SIP_BODY_TEXT_PLAIN; };

	bool parse(char *sipMsg) { text = createString(sipMsg); return true; };
	bool encode(char *sipMsg) { strcat(sipMsg, text); return true; };

	void setText(const char *text);
	const char *getText() { return text; };
};

__END_SYS

#endif
