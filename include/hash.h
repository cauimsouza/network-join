#ifndef HASH_CPP
#define HASH_CPP

#include <cmath>

inline unsigned mod_hash(unsigned k, unsigned size)
{
	return k % size;
}

inline unsigned mult_hash(unsigned k,
		   unsigned size,
		   double A = (std::sqrt(5) - 1)/2)
{
	double x = k * A;
	x -= std::trunc(x);
	x *= size;
	
	return (unsigned) std::trunc(x);
}

#endif
