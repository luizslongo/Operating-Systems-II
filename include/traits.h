#ifndef __traits_h
#define __traits_h

#include <system/config.h>

#ifdef __APPLICATION_TRAITS_H
#include __APPLICATION_TRAITS_H
#else
#include <system/traits.h>
#endif

#ifdef __ARCH_TRAITS_H
#include __ARCH_TRAITS_H
#endif

#ifdef __MACH_TRAITS_H
#include __MACH_TRAITS_H
#endif

#endif
