// EPOS OStream Interface

#include <system/config.h>

#ifndef __ostream_h
#define __ostream_h

__BEGIN_SYS

// OStream and StringStream are built using the CRTP
// ref: http://en.wikipedia.org/wiki/Curiously_Recurring_Template_Pattern

class BaseStream
{
public:
    struct Endl {};
    struct Hex {};
    struct Dec {};
    struct Oct {};
    struct Bin {};
    
public:
    BaseStream(): _base(10) {}

protected:
    int itoa(int v, char * s);
    int utoa(unsigned int v, char * s, unsigned int i = 0);
    int llitoa(long long int v, char * s);
    int llutoa(unsigned long long int v, char * s, unsigned int i = 0);
    int ptoa(const void * p, char * s);

protected:
    int _base;

    static const char _digits[];
};

template<typename T>
class ConcreteStream : public BaseStream {

public:
    T & operator<<(const Endl & endl) {
	static_cast<T*>(this)->print("\n");
	_base = 10;
	static_cast<T*>(this)->flush();
	return *static_cast<T*>(this);
    }

    T & operator<<(const Hex & hex) {
	_base = 16;
	return *static_cast<T*>(this);
    }
    T & operator<<(const Dec & dec) {
	_base = 10;
	return *static_cast<T*>(this);
    }
    T & operator<<(const Oct & oct) {
	_base = 8;
	return *static_cast<T*>(this);
    }
    T & operator<<(const Bin & bin) {
	_base = 2;
	return *static_cast<T*>(this);
    }

    T & operator<<(char c) {
	char buf[2];
	buf[0] = c;
	buf[1] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }
    
    T & operator<<(unsigned char c) { 
	return operator<<(static_cast<char>(c));
    }

    T & operator<<(int i) {
	char buf[64];
	buf[itoa(i, buf)] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }
    T & operator<<(short s) {
	return operator<<(static_cast<int>(s));
    }
    
    T & operator<<(long l) {
	return operator<<(static_cast<int>(l));
    }

    T & operator<<(unsigned int u) {
	char buf[64];
	buf[utoa(u, buf)] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }
    
    T & operator<<(unsigned short s) { 
	return operator<<(static_cast<unsigned int>(s));
    }
    
    T & operator<<(unsigned long l) {
	return operator<<(static_cast<unsigned int>(l));
    }

    T & operator<<(long long int u) {
	char buf[64];
	buf[llitoa(u, buf)] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }

    T & operator<<(unsigned long long int u) {
	char buf[64];
	buf[llutoa(u, buf)] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }

    T & operator<<(const void * p) {
	char buf[64];
	buf[ptoa(p, buf)] = '\0';
	static_cast<T*>(this)->print(buf);
	return *static_cast<T*>(this);
    }

    T & operator<<(const char * s) { 
	static_cast<T*>(this)->print(s);
	return *static_cast<T*>(this); 
    }

};

class OStream;
class OStream : public ConcreteStream<OStream>
{
public:
    void flush() {}
    void print(const char * s);
};

class StringStream;
class StringStream : public ConcreteStream<StringStream> {
protected:
    char * _str;
    int _size;
    int _written;
    
public:
    StringStream(char * s,int l) : _str(s), _size(l), _written(0) {}

    int written() { return _written; }
    void end() { if (_size) *_str = '\0'; }
    void print(const char * s);
    void flush() {}

};

extern BaseStream::Endl endl;
extern BaseStream::Hex hex;
extern BaseStream::Dec dec;
extern BaseStream::Oct oct;
extern BaseStream::Bin bin;

extern OStream kout, kerr;

__END_SYS

#endif
