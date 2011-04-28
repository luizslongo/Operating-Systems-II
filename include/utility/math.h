// EPOS Math Utility Declarations

#ifndef __math_h
#define __math_h

#include <system/config.h>

__BEGIN_SYS

namespace Math {

static const float E = 2.71828183;

float logf(float num, float base = E, float epsilon = 1e-12) {
    float integer = 0;
    if (num == 0) return 1;

    if (num < 1  && base < 1) return 0;

    while (num < 1) {
        integer--;
        num *= base;
    }

    while (num >= base) {
        integer++;
        num /= base;
    }

    float partial = 0.5;
    num *= num;
    float decimal = 0.0;
    while (partial > epsilon) {
        if (num >= base) {
            decimal += partial;
            num /= base;
        }
        partial *= 0.5;
        num *= num;
    }
    return (integer + decimal);
}

};


__END_SYS

#endif
