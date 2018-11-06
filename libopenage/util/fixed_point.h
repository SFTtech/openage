// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <algorithm>
#include <climits>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>

#include "misc.h"

namespace openage {
namespace util {


/**
 * Helper function that performs a left shift without causing undefined
 * behavior.
 * regular left-shift is undefined if amount >= bitwidth,
 * or amount >= bitwidth - 1 for signed integers.
 */
template<unsigned int amount, typename T>
constexpr static
typename std::enable_if<(amount + (std::is_signed<T>::value ? 1 : 0) < sizeof(T) * CHAR_BIT), T>::type
safe_shiftleft(T value) {
	return static_cast<T>(
		static_cast<typename std::make_unsigned<T>::type>(value) << amount
	);
}


/**
 * Helper function that performs a right shift without causing undefined
 * behavior.
 * right-shift is usually undefined if amount >= bit size.
 */
template<unsigned int amount, typename T>
constexpr static
typename std::enable_if<(amount >= sizeof(T) * CHAR_BIT), T>::type
safe_shiftright(T value) {
	return value < 0 ? -1 : 0;
}

template<unsigned int amount, typename T>
constexpr static
typename std::enable_if<(amount < sizeof(T) * CHAR_BIT), T>::type
safe_shiftright(T value) {
	return value >> amount;
}


/**
 * Helper function that performs either a safe shift-right (amount > 0),
 * or a safe shift-left (amount < 0).
 */
template<int amount, typename T>
constexpr static
typename std::enable_if<(amount < 0), T>::type
safe_shift(T value) {
	return safe_shiftright<-amount>(value);
}


template<int amount, typename T>
constexpr static
typename std::enable_if<(amount >= 0), T>::type
safe_shift(T value) {
	return safe_shiftleft<amount>(value);
}


/**
 * Fixed-point integer class;
 *
 * this is designed to be used instead of floats in places where guaranteed
 * precision is required.
 *
 * For example,
 * FixedPoint<int64_t, 32>
 * can store values from -2**32 to +2**32 with a constant precision of 2**-32.
 */
template<typename int_type, unsigned int fractional_bits>
class FixedPoint {
public:
	using raw_type = int_type;
	using this_type = FixedPoint<int_type, fractional_bits>;
	using unsigned_int_type = typename std::make_unsigned<int_type>::type;
	using same_type_but_unsigned = FixedPoint<typename FixedPoint::unsigned_int_type,
	                                          fractional_bits>;

private:
	// Helper function to create the scaling factors that are used below.
	static constexpr double power_of_two(unsigned int power) {
		double result = 1.0;
		while (power--) {
			result *= 2.0;
		}
		return result;
	}

	/**
	 * Storage of the fixed point data.
	 */
	int_type raw_value;

	static constexpr const double from_double_factor = power_of_two(fractional_bits);
	static constexpr const double to_double_factor = 1 / from_double_factor;
	static constexpr const float from_float_factor = from_double_factor;
	static constexpr const float to_float_factor = to_double_factor;

	static constexpr const unsigned int approx_decimal_places = static_cast<unsigned int>(
		static_cast<double>(fractional_bits) * 0.30103 + 1
	);

	// constexpr helper function for get_fractional_part()
	static constexpr typename FixedPoint::unsigned_int_type fractional_part_bitmask() {
		// return ~(MAX_VAL << fractional_bits);
		return static_cast<FixedPoint::unsigned_int_type>(
			~(
				safe_shiftleft<fractional_bits, FixedPoint::unsigned_int_type>(
					std::numeric_limits<FixedPoint::unsigned_int_type>::max()
		)));
	}

	friend std::hash<openage::util::FixedPoint<int_type, fractional_bits>>;

	static constexpr int_type raw_value_from_double(double n) {
		return static_cast<int_type>(n * from_double_factor);
	}

public:
	// obligatory copy constructor / assignment operator.
	constexpr FixedPoint(const FixedPoint &other)
		:
		raw_value(other.raw_value) {}

	constexpr FixedPoint(FixedPoint &&other) noexcept
		:
		raw_value(std::move(other.raw_value)) {}

	constexpr FixedPoint &operator =(const FixedPoint &other) {
		this->raw_value = other.raw_value;
		return *this;
	}

	constexpr FixedPoint &operator =(FixedPoint &&other) noexcept {
		this->raw_value = std::move(other.raw_value);
		return *this;
	}

	/**
	 * Empty constructor. Initializes the number to 0.
	 */
	constexpr FixedPoint()
		:
		raw_value(0) {}

	/**
	 * floating-point constructor. Initializes the number from a double.
	 */
	// implicitly construct from double.
	// for other creations, use the factory methods below.
	constexpr FixedPoint(double n) : raw_value(FixedPoint::raw_value_from_double(n)) {}

	/**
	 * FixedPoint value that is preinitialized to zero.
	 */
	static constexpr FixedPoint zero() {
		return FixedPoint::from_int(0);
	}

	/**
	 * Factory function to get a fixed-point number from an integer.
	 */
	static constexpr FixedPoint from_int(int_type n) {
		return FixedPoint::from_raw_value(safe_shiftleft<fractional_bits, int_type>(n));
	}

	/**
	 * Factory function to get a fixed-point number from a float.
	 */
	static constexpr FixedPoint from_float(float n) {
		return FixedPoint::from_raw_value(static_cast<int_type>(n * from_float_factor));
	}

	/**
	 * Factory function to get a fixed-point number from a double.
	 */
	static constexpr FixedPoint from_double(double n) {
		return FixedPoint::from_raw_value(FixedPoint::raw_value_from_double(n));
	}

	/**
	 * Factory function to get a fixed-point number from a fixed-point number of different type.
	 */
	template<typename other_int_type, unsigned int other_fractional_bits>
	static constexpr FixedPoint from_fixedpoint(const FixedPoint<other_int_type, other_fractional_bits> &other) {
		return FixedPoint::from_raw_value(
			safe_shift<fractional_bits - other_fractional_bits, int_type>(other.get_raw_value())
		);
	}

	/**
	 * The minimum possible value of this type.
	 */
	static constexpr const FixedPoint min_value() {
		return FixedPoint::from_raw_value(std::numeric_limits<int_type>::min());
	}

	/**
	 * The maximum possible value of this type.
	 */
	static constexpr const FixedPoint max_value() {
		return FixedPoint::from_raw_value(std::numeric_limits<int_type>::max());
	}

	/**
	 * Factory function to construct a fixed-point number with a given raw value.
	 * Don't use this.
	 */
	static constexpr FixedPoint from_raw_value(int_type raw_value) {
		FixedPoint result;
		result.raw_value = raw_value;
		return result;
	}

	/**
	 * Converter to retrieve the raw value of the fixed-point number.
	 * Don't use this.
	 */
	constexpr int_type get_raw_value() const {
		return this->raw_value;
	}

	/**
	 * Converter to retrieve the int (pre-decimal) part of the number.
	 */
	constexpr int_type to_int() const {
		return safe_shiftright<fractional_bits, int_type>(this->raw_value);
	}

	constexpr explicit operator int() const {
		return this->to_int();
	}

	/**
	 * Converter to retrieve the number as float.
	 */
	constexpr float to_float() const {
		return static_cast<float>(this->raw_value) * FixedPoint::to_float_factor;
	}

	constexpr explicit operator float() const {
		return this->to_float();
	}

	/**
	 * Converter to retrieve the number as double.
	 */
	constexpr double to_double() const {
		return static_cast<double>(this->raw_value) * FixedPoint::to_double_factor;
	}

	constexpr explicit operator double() const {
		return this->to_double();
	}

	/**
	 * Show a string representation. Useful for debugging in gdb.
	 */
	std::string str() const {
		std::ostringstream builder;
		builder << "FixedPoint(" << this->to_double()
		        << ", fracbits=" << fractional_bits
		        << ", raw=" << this->raw_value
		        << ")";
		return builder.str();
	};

	/**
	 * Converter to retrieve the fractional (post-decimal) part of the number.
	 */
	constexpr typename FixedPoint::same_type_but_unsigned get_fractional_part() const {
		// returns a new variable with only the bits from
		// fractional_part_bitmask set.
		return FixedPoint::same_type_but_unsigned::from_raw_value(
			static_cast<FixedPoint::unsigned_int_type>(this->raw_value) &
			std::integral_constant<
				int_type,
				FixedPoint::fractional_part_bitmask()
			>::value
		);
	}

	// Comparison operators for comparison with other
	constexpr bool operator ==(const FixedPoint &o) const {
		return raw_value == o.raw_value;
	}

	constexpr bool operator !=(const FixedPoint &o) const {
		return raw_value != o.raw_value;
	}

	constexpr bool operator <(const FixedPoint &o) const {
		return raw_value < o.raw_value;
	}

	constexpr bool operator >(const FixedPoint &o) const {
		return raw_value > o.raw_value;
	}

	constexpr bool operator <=(const FixedPoint &o) const {
		return raw_value <= o.raw_value;
	}

	constexpr bool operator >=(const FixedPoint &o) const {
		return raw_value >= o.raw_value;
	}

	// Unary operators
	constexpr FixedPoint operator +() const {
		return *this;
	}

	// the inner_int_type template is required for enable_if.
	template <typename inner_int_type=int_type>
	constexpr
	typename std::enable_if<std::is_signed<inner_int_type>::value, typename FixedPoint::this_type>::type
	operator -() const {
		static_assert(std::is_same<inner_int_type, int_type>::value, "inner_int_type must == int_type");
		return FixedPoint::this_type::from_raw_value(-this->raw_value);
	}

	template<typename I, unsigned F>
	constexpr double hypot(const FixedPoint<I, F> rhs) {
		return std::hypot(this->to_double(), rhs.to_double());
	}

	template<typename I, unsigned F>
	constexpr FixedPoint<I, F> hypotfp(const FixedPoint<I, F> rhs) {
		return FixedPoint<I, F>(this->hypot(rhs));
	}

	// Basic operators
	constexpr FixedPoint &operator +=(const FixedPoint &n) {
		this->raw_value += n.raw_value;
		return *this;
	}

	constexpr FixedPoint &operator -=(const FixedPoint &n) {
		this->raw_value -= n.raw_value;
		return *this;
	}

	/**
	 * FixedPoint *= N, where N is not a FixedPoint.
	 */
	template<typename N>
	typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint &>::type
	constexpr operator *=(const N &rhs) {
		this->raw_value *= rhs;
		return *this;
	}

	/**
	 * FixedPoint /= N
	 */
	template<typename N>
	constexpr FixedPoint &operator /=(const N &rhs) {
		this->raw_value = div(this->raw_value, static_cast<int_type>(rhs));
		return *this;
	}

	void swap(FixedPoint &rhs) {
		std::swap(this->raw_value, rhs.raw_value);
	}

	// I/O operators
	friend std::ostream &operator <<(std::ostream &os, const FixedPoint &n) {
		os << std::fixed << std::setprecision(FixedPoint::approx_decimal_places) << double(n);
		return os;
	}

	friend std::istream &operator >>(std::istream &is, FixedPoint &n) {
		double temp;
		is >> temp;
		n = temp;
		return is;
	}

	constexpr double sqrt() {
		return std::sqrt(this->to_double());
	}
};


// Binary operators

/**
 * FixedPoint + FixedPoint
 */
template<typename I, unsigned int F>
constexpr FixedPoint<I, F> operator +(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> &rhs) {
	return FixedPoint<I, F>::from_raw_value(lhs.get_raw_value() + rhs.get_raw_value());
}

/**
 * FixedPoint + double
 */
template<typename I, unsigned int F>
constexpr FixedPoint<I, F> operator +(const FixedPoint<I, F> &lhs, const double &rhs) {
	return FixedPoint<I, F>{lhs} + FixedPoint<I, F>::from_double(rhs);
}

/**
 * FixedPoint - FixedPoint
 */
template<typename I, unsigned int F>
constexpr FixedPoint<I, F> operator -(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> &rhs) {
	return FixedPoint<I, F>::from_raw_value(lhs.get_raw_value() - rhs.get_raw_value());
}

/**
 * FixedPoint - double
 */
template<typename I, unsigned int F>
constexpr FixedPoint<I, F> operator -(const FixedPoint<I, F> &lhs, const double &rhs) {
	return FixedPoint<I, F>{lhs} - FixedPoint<I, F>::from_double(rhs);
}


/**
 * FixedPoint * N
 */
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
constexpr operator *(const FixedPoint<I, F> lhs, const N &rhs) {
	return FixedPoint<I, F>::from_raw_value(lhs.get_raw_value() * rhs);
}

/*
 FixedPoint * FixedPoint is missing to prevent surprising overflows.

 using fp = FixedPoint<uint64_t, 16>;
 fp a = fp.from_int(1 << 16);
 => a * a will overflow because:
    a.rawvalue == 2^(16+16) == 2^32
    -> a.rawvalue * a.rawvalue == 2^64 => pwnt
*/


/**
 * FixedPoint / N
 */
template<typename I, unsigned F, typename N>
constexpr FixedPoint<I, F> operator /(const FixedPoint<I, F> lhs, const N &rhs) {
	return FixedPoint<I, F>::from_raw_value(div(lhs.get_raw_value(), static_cast<I>(rhs)));
}

}} // namespace openage::util


// std function overloads
namespace std {

template<typename I, unsigned F>
constexpr double sqrt(openage::util::FixedPoint<I, F> n) {
	return n.sqrt();
}

template<typename I, unsigned F>
constexpr openage::util::FixedPoint<I, F> min(openage::util::FixedPoint<I, F> x, openage::util::FixedPoint<I, F> y) {
	return openage::util::FixedPoint<I, F>::from_raw_value(
		std::min(x.get_raw_value(),
		         y.get_raw_value())
	);
}

template<typename I, unsigned F>
constexpr openage::util::FixedPoint<I, F> max(openage::util::FixedPoint<I, F> x, openage::util::FixedPoint<I, F> y) {
	return openage::util::FixedPoint<I, F>::from_raw_value(
		std::max(x.get_raw_value(),
		         y.get_raw_value())
	);
}

template<typename I, unsigned F>
constexpr openage::util::FixedPoint<I, F> abs(openage::util::FixedPoint<I, F> n) {
	return openage::util::FixedPoint<I, F>::from_raw_value(
		std::abs(n.get_raw_value())
	);
}

template<typename I, unsigned F>
constexpr double hypot(openage::util::FixedPoint<I, F> x, openage::util::FixedPoint<I, F> y) {
	return x.hypot(y);
}

template<typename I, unsigned F>
struct hash<openage::util::FixedPoint<I, F>> {
	constexpr size_t operator ()(const openage::util::FixedPoint<I, F> &n) const {
		return std::hash<I>{}(n.raw_value);
	}
};

template<typename I, unsigned F>
struct numeric_limits<openage::util::FixedPoint<I, F>> {
	constexpr static openage::util::FixedPoint<I, F> min() {
		return openage::util::FixedPoint<I, F>::min_value();
	}

	constexpr static openage::util::FixedPoint<I, F> max() {
		return openage::util::FixedPoint<I, F>::max_value();
	}
};

} // namespace std
