#ifndef _ENGINE_UTIL_MISC_H_
#define _ENGINE_UTIL_MISC_H_

namespace engine {
namespace util {

/**
returns a random number in the given range
@param lower the (included) lower limit of the generated number
@param upper the (excluded) upper limit of the generated number
*/
int random_range(int lower, int upper);

/**
implements the 'correct' version of the modulo operator,
which always returns positive values
*/
template <typename T>
T mod(T x, T m) {
	T r = x % m;

	if(r < 0) {
		return r + m;
	} else {
		return r;
	}
}

/**
implements the 'correct' version of the division operator,
which always rounds to -inf
*/
template <typename T>
T div(T x, T m) {
	return (x - mod<T>(x, m)) / m;
}

/**
generic callable, that compares any types for creating a total order.

use for stdlib structures like std::set
*/
template <typename T>
struct less {
	bool operator ()(const T x, const T y) {
		return *x < *y;
	}
};

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_MISC_H_
