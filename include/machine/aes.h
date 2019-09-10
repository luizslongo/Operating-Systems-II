// EPOS AES Mediator Common Package

#ifndef __aes_h
#define __aes_h

#include <system/config.h>

__BEGIN_SYS

class AES_Common
{
protected:
    AES_Common() {}
};

__END_SYS

#ifndef __AES_H
// If the machine does not feature an AES accelerator, then use a software lib
#include <utility/aes.h>

__BEGIN_SYS

template<unsigned int KEY_SIZE>
using AES = _UTIL::_AES<KEY_SIZE>;

__END_SYS
#endif

#endif

#if defined(__AES_H) && !defined(__common_only__)
#include __AES_H

__BEGIN_SYS

template<unsigned int KEY_SIZE>
using AES = _SYS::_AES<KEY_SIZE>;

__END_SYS

#endif
