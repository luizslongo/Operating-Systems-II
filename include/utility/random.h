// EPOS-- Random Number Generator Utility Declarations

#ifndef __random_h
#define __random_h

#include <system/config.h>

__BEGIN_SYS

// Implementation of http://en.wikipedia.org/wiki/Linear_congruential_generator
// with A = 2 and variable C on a simplification of:
// X1 = aX0 + c
// as X1 = (X0 << 1) xor C
class Pseudo_Random
{
public:
    Pseudo_Random() {}
	
    static unsigned long int random(unsigned long int n) { 
	_seed = (_seed << 1) ^ n;
	return _seed;
    }

private:
    static unsigned long int _seed;
};

__END_SYS
#endif
