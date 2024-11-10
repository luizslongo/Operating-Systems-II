#ifndef __ostream_wrapped_h
#define __ostream_wrapped_h

#include<system/config.h>
#include<utility/ostream.h>
#include<utility/spin.h>

extern "C" {
    void _print_preamble();
    void _print(const char * s);
    void _print_trailler(bool error);
}

__BEGIN_UTIL
extern Simple_Spin _ostream_lock;
class OStream_Wrapped : public OStream {
    protected:
    void print(const char *s) {
        _ostream_lock.acquire();
        _print(s);
        _ostream_lock.release();
    }
};
__END_UTIL

#endif