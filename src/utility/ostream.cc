// EPOS OStream Implementation

#include <utility/ostream.h>
#include <display.h>

__BEGIN_SYS

const char OStream::_digits[] = "0123456789abcdef";

void OStream::print(const char * s)
{
//     static volatile bool lock = false;

//     if(Traits<Thread>::smp) while(CPU::tsl(lock));
    Display::puts(s); 
//     if(Traits<Thread>::smp) lock = false;
}

int OStream::itoa(int v, char * s)
{
    unsigned int i = 0;

    if(v < 0) {
	v = -v;
	s[i++] = '-';
    }

    return utoa(static_cast<unsigned int>(v), s, i);
} 

int OStream::utoa(unsigned int v, char * s, unsigned int i)
{
    unsigned int j;

    if(!v) {
	s[i++] = '0';
	return i;
    }

    if(v > 256) {
	if(_base == 8 || _base == 16)
	    s[i++] = '0';
	if(_base == 16)
	    s[i++] = 'x';
    }

    for(j = v; j != 0; i++, j /= _base);
    for(j = 0; v != 0; j++, v /= _base)
	s[i - 1 - j] = _digits[v % _base];

    return i;
}

int OStream::llitoa(long long int v, char * s)
{
    unsigned int i = 0;

    if(v < 0) {
	v = -v;
	s[i++] = '-';
    }

    return llutoa(static_cast<unsigned long long int>(v), s, i);
} 

int OStream::llutoa(unsigned long long int v, char * s, unsigned int i)
{
    unsigned int j;

    if(!v) {
	s[i++] = '0';
	return i;
    }

    if(v > 256) {
	if(_base == 8 || _base == 16)
	    s[i++] = '0';
	if(_base == 16)
	    s[i++] = 'x';
    }

    for(j = v; j != 0; i++, j /= _base);
    for(j = 0; v != 0; j++, v /= _base)
	s[i - 1 - j] = _digits[v % _base];

    return i;
}

int OStream::ptoa(const void * p, char * s)
{
    unsigned int j, v = reinterpret_cast<unsigned int>(p);

    s[0] = '0';
    s[1] = 'x';

    for(j = 0; j < sizeof(void *) * 2; j++, v >>= 4)
	s[2 + sizeof(void *) * 2 - 1 - j]
	    = _digits[v & 0xf];

    return j + 2;
}    

void StringStream::print(const char * s) {
    while (*s && _size) { 
        *_str++ = *s++;
        _written++; _size--;
    }
}

__END_SYS
