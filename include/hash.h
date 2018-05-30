/*
 * Created in Palaiseau (France) in 30/05/2018 by:
 * Cauim de S. Lima (cauimsouza@gmail.com)
 * Victor Hugo V. Silva (vitorvianna10@gmail.com)
 */

#ifndef HASH_CPP
#define HASH_CPP

#include <cmath>
#include "MurmurHash3.h"

inline unsigned mod_hash(unsigned key, unsigned size)
{
	return key % size;
}

inline unsigned mult_hash(unsigned key,
		   unsigned size,
		   double A = (std::sqrt(5) - 1)/2)
{
	double x = key * A;
	x -= std::trunc(x);
	x *= size;
	
	return (unsigned) std::trunc(x);
}

inline unsigned murmur_hash(unsigned key, unsigned size)
{
	unsigned result = 0;
	MurmurHash3_x86_32(&key, sizeof key, 0, &result);

	return result % size;
}

#endif
