#ifndef OPENAGE_UTIL_MISC_H_
#define OPENAGE_UTIL_MISC_H_

namespace openage {
namespace util {

/**
 * returns a random number in the given range
 * @param lower the (included) lower limit of the generated number
 * @param upper the (excluded) upper limit of the generated number
 */
int random_range(int lower, int upper);

/**
 * modulo operation that guarantees to return positive values.
 */
template <typename T>
T mod(T x, T m) {
	T r = x % m;

	if (r < 0) {
		return r + m;
	} else {
		return r;
	}
}

/**
 * compiletime defined modulo function.
 */
template <typename T, unsigned int modulo>
T mod(T x) {
	T r = x % modulo;

	if (r < 0) {
		return r + modulo;
	} else {
		return r;
	}
}


/**
 * compiletime defined rotate left function
 */
template <typename T, int amount>
T rol(T x) {
	static_assert(sizeof(T) > amount && amount > 0, "invalid rotation amount");
	return (x << amount) | (x >> (sizeof(T)*8 - amount));
}


/**
 * implements the 'correct' version of the division operator,
 * which always rounds to -inf
 */
template <typename T>
inline T div(T x, T m) {
	return (x - mod<T>(x, m)) / m;
}

/**
 * generic callable, that compares any types for creating a total order.
 *
 * use for stdlib structures like std::set.
 * the template paramter has to be a pointer type.
 */
template <typename T>
struct less {
	bool operator ()(const T x, const T y) const {
		return *x < *y;
	}
};

} //namespace util
} //namespace openage

#endif
