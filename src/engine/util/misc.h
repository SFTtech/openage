#ifndef _ENGINE_UTIL_MISC_H_
#define _ENGINE_UTIL_MISC_H_

namespace engine {
namespace util {

/**
returns a random number in the given range
@param lower the (included) lower limit of the generated number
@param upper the (also included) upper limit of the generated number
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

} //namespace util
} //namespace engine

#endif //_ENGINE_UTIL_MISC_H_
