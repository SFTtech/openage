// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_MATH_H_
#define OPENAGE_UTIL_MATH_H_

#include <cmath>

namespace openage {
namespace math {


/**
 * Approximate a float square root by magic.
 */
float reciprocal_sqrt(float x) {
	long i;
	float y, r;

	y = x * 0.5f;
	i = *(long *)(&x);
	i = 0x5f3759df - (i >> 1); // magic!
	r = *(float *)(&i);
	r = r * (1.5f - (r * r * y));
	return r;
}

}} // openage::util

#endif
