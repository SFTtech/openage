// Copyright 2015-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <algorithm>
#include <bit>
#include <climits>
#include <cmath>
#include <iomanip>
#include <limits>
#include <ostream>
#include <type_traits>

#include "compiler.h"
#include "misc.h"

namespace openage {
namespace util {


/**
 * Helper function that performs a left shift without causing undefined
 * behavior.
 * regular left-shift is undefined if amount >= bitwidth,
 * or amount >= bitwidth - 1 for signed integers.
 */
template <unsigned int amount, typename T>
constexpr static
	typename std::enable_if<(amount + (std::is_signed<T>::value ? 1 : 0) < sizeof(T) * CHAR_BIT), T>::type
	safe_shiftleft(T value) {
	return static_cast<T>(
		static_cast<typename std::make_unsigned<T>::type>(value) << amount);
}


/**
 * Helper function that performs a right shift without causing undefined
 * behavior.
 * right-shift is usually undefined if amount >= bit size.
 */
template <unsigned int amount, typename T>
constexpr static
	typename std::enable_if<(amount >= sizeof(T) * CHAR_BIT), T>::type
	safe_shiftright(T value) {
	return value < 0 ? -1 : 0;
}

template <unsigned int amount, typename T>
constexpr static
	typename std::enable_if<(amount < sizeof(T) * CHAR_BIT), T>::type
	safe_shiftright(T value) {
	return value >> amount;
}


/**
 * Helper function that performs either a safe shift-right (amount < 0),
 * or a safe shift-left (amount >= 0).
 */
template <int amount, typename T>
constexpr static
	typename std::enable_if<(amount < 0), T>::type
	safe_shift(T value) {
	return safe_shiftright<-amount>(value);
}


template <int amount, typename T>
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
 *
 * If you change this class, remember to update the gdb pretty printers
 * in etc/gdb_pretty/printers.py.
 */
template <typename int_type, unsigned int fractional_bits, typename intermediate_type = int_type>
class FixedPoint {
public:
	using raw_type = int_type;
	using this_type = FixedPoint<int_type, fractional_bits, intermediate_type>;
	using unsigned_int_type = typename std::make_unsigned<int_type>::type;
	using unsigned_intermediate_type = typename std::make_unsigned<intermediate_type>::type;

	using same_type_but_unsigned = FixedPoint<typename FixedPoint::unsigned_int_type,
	                                          fractional_bits, typename FixedPoint::unsigned_intermediate_type>;

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
		static_cast<double>(fractional_bits) * 0.30103 + 1);

	// constexpr helper function for get_fractional_part()
	static constexpr typename FixedPoint::unsigned_int_type fractional_part_bitmask() {
		// return ~(MAX_VAL << fractional_bits);
		return static_cast<FixedPoint::unsigned_int_type>(
			~(
				safe_shiftleft<fractional_bits, FixedPoint::unsigned_int_type>(
					std::numeric_limits<FixedPoint::unsigned_int_type>::max())));
	}

	friend std::hash<openage::util::FixedPoint<int_type, fractional_bits>>;

	static constexpr int_type raw_value_from_double(double n) {
		return static_cast<int_type>(n * from_double_factor);
	}

public:
	// obligatory copy constructor / assignment operator.
	constexpr FixedPoint(const FixedPoint &other) :
		raw_value(other.raw_value) {}

	constexpr FixedPoint(FixedPoint &&other) noexcept
		:
		raw_value(std::move(other.raw_value)) {}

	constexpr FixedPoint &operator=(const FixedPoint &other) {
		this->raw_value = other.raw_value;
		return *this;
	}

	constexpr FixedPoint &operator=(FixedPoint &&other) noexcept {
		this->raw_value = std::move(other.raw_value);
		return *this;
	}

	/**
	 * Empty constructor. Initializes the number to 0.
	 */
	constexpr FixedPoint() :
		raw_value(0) {}

	/**
	 * floating-point constructor. Initializes the number from a double.
	 */
	// implicitly construct from double.
	// for other creations, use the factory methods below.
	constexpr FixedPoint(double n) :
		raw_value(FixedPoint::raw_value_from_double(n)) {}

	/**
	 * FixedPoint value that is preinitialized to zero.
	 */
	static constexpr FixedPoint zero() {
		return FixedPoint::from_int(0);
	}

	/**
	 * Math constants represented in FixedPoint
	 */
	// naming, definition and value are kept compatible with `math_constants.h`
	static constexpr FixedPoint e() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(6267931151224907085ll));
	}

	static constexpr FixedPoint log2e() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(3326628274461080622ll));
	}

	static constexpr FixedPoint log10e() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(1001414895036696345ll));
	}

	static constexpr FixedPoint ln2() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(1598288580650331957ll));
	}

	static constexpr FixedPoint ln10() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(5309399739799983627ll));
	}

	static constexpr FixedPoint pi() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(7244019458077122842ll));
	}

	static constexpr FixedPoint pi_2() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(3622009729038561421ll));
	}

	static constexpr FixedPoint pi_4() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(1811004864519280710ll));
	}

	static constexpr FixedPoint inv_pi() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(733972625820500306ll));
	}

	static constexpr FixedPoint inv2_pi() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(1467945251641000613ll));
	}

	static constexpr FixedPoint inv2_sqrt_pi() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(2601865214189558307ll));
	}

	static constexpr FixedPoint tau() {
		return from_fixedpoint(FixedPoint<int64_t, 60>::from_raw_value(7244019458077122842ll));
	}

	static constexpr FixedPoint degs_per_rad() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(40244552544872904ll));
	}

	static constexpr FixedPoint rads_per_deg() {
		return from_fixedpoint(FixedPoint<int64_t, 57>::from_raw_value(8257192040480628449ll));
	}

	static constexpr FixedPoint sqrt_2() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(3260954456333195553ll));
	}

	static constexpr FixedPoint inv_sqrt_2() {
		return from_fixedpoint(FixedPoint<int64_t, 61>::from_raw_value(1630477228166597776ll));
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
	template <typename other_int_type, unsigned int other_fractional_bits, typename other_intermediate_type, typename std::enable_if<(fractional_bits > other_fractional_bits)>::type * = nullptr>
	static constexpr FixedPoint from_fixedpoint(const FixedPoint<other_int_type, other_fractional_bits, other_intermediate_type> &other) {
		return FixedPoint::from_raw_value(
			safe_shift<fractional_bits - other_fractional_bits, int_type>(static_cast<int_type>(other.get_raw_value())));
	}

	template <typename other_int_type, unsigned int other_fractional_bits, typename other_intermediate_type, typename std::enable_if<(fractional_bits <= other_fractional_bits)>::type * = nullptr>
	static constexpr FixedPoint from_fixedpoint(const FixedPoint<other_int_type, other_fractional_bits, other_intermediate_type> &other) {
		return FixedPoint::from_raw_value(
			static_cast<int_type>(other.get_raw_value() / safe_shiftleft<other_fractional_bits - fractional_bits, other_int_type>(1)));
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
			static_cast<FixedPoint::unsigned_int_type>(this->raw_value) & std::integral_constant<int_type, FixedPoint::fractional_part_bitmask()>::value);
	}

	// Comparison operators for comparison with other
	constexpr auto operator<=>(const FixedPoint &o) const = default;

	// Unary operators
	constexpr FixedPoint operator+() const {
		return *this;
	}

	// the inner_int_type template is required for enable_if.
	template <typename inner_int_type = int_type>
	constexpr
		typename std::enable_if<std::is_signed<inner_int_type>::value, typename FixedPoint::this_type>::type
		operator-() const {
		static_assert(std::is_same<inner_int_type, int_type>::value, "inner_int_type must == int_type");
		return FixedPoint::this_type::from_raw_value(-this->raw_value);
	}

	template <typename I, unsigned F, typename Inter>
	constexpr double hypot(const FixedPoint<I, F, Inter> rhs) {
		return std::hypot(this->to_double(), rhs.to_double());
	}

	template <typename I, unsigned F, typename Inter>
	constexpr FixedPoint<I, F> hypotfp(const FixedPoint<I, F, Inter> rhs) {
		return FixedPoint<I, F, Inter>(this->hypot(rhs));
	}

	// Basic operators
	constexpr FixedPoint &operator+=(const FixedPoint &n) {
		this->raw_value += n.raw_value;
		return *this;
	}

	constexpr FixedPoint &operator-=(const FixedPoint &n) {
		this->raw_value -= n.raw_value;
		return *this;
	}

	/**
	 * FixedPoint *= N, where N is not a FixedPoint.
	 */
	template <typename N>
	typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint &>::type constexpr operator*=(const N &rhs) {
		this->raw_value *= rhs;
		return *this;
	}

	/**
	 * FixedPoint /= N
	 */
	template <typename N>
	constexpr FixedPoint &operator/=(const N &rhs) {
		this->raw_value = div(this->raw_value, static_cast<int_type>(rhs));
		return *this;
	}

	void swap(FixedPoint &rhs) {
		std::swap(this->raw_value, rhs.raw_value);
	}

	// I/O operators
	friend std::ostream &operator<<(std::ostream &os, const FixedPoint &n) {
		os << std::fixed << std::setprecision(FixedPoint::approx_decimal_places) << double(n);

		if (n == FixedPoint::max_value()) [[unlikely]] {
			os << "[MAX]";
		}
		else if (n != 0 and n == FixedPoint::min_value()) [[unlikely]] {
			os << "[MIN]";
		}

		return os;
	}

	friend std::istream &operator>>(std::istream &is, FixedPoint &n) {
		double temp;
		is >> temp;
		n = temp;
		return is;
	}

	/**
	 * Pure FixedPoint sqrt implementation using Heron's Algorithm.
	 *
	 * Note that this function is undefined for negative values.
	 *
	 * There's a small loss in precision depending on the value of fractional_bits and the position of
	 * the most significant bit: if the integer portion is very large, we won't have as much (absolute)
	 * precision. Ideally you would want the intermediate_type to be twice the size of raw_type to avoid
	 * any losses.
	 */
	constexpr FixedPoint sqrt() {
		// Zero can cause issues later, so deal with now.
		if (this->raw_value == 0) {
			return zero();
		}

		// Check for negative values
		if constexpr (std::is_signed<raw_type>()) {
			ENSURE(this->raw_value > 0, "FixedPoint::sqrt() is undefined for negative values.");
		}

		// A greater shift = more precision, but can overflow the intermediate type if too large.
		size_t max_shift = std::countl_zero(static_cast<unsigned_intermediate_type>(this->raw_value)) - 1;
		size_t shift = max_shift > fractional_bits ? fractional_bits : max_shift;

		// shift + fractional bits must be an even number
		if ((shift + fractional_bits) % 2) {
			shift -= 1;
		}

		// We can't use the safe shift since the shift value is unknown at compile time.
		intermediate_type n = static_cast<intermediate_type>(this->raw_value) << shift;
		intermediate_type guess = static_cast<intermediate_type>(1) << fractional_bits;

		for (size_t i = 0; i < fractional_bits; i++) {
			intermediate_type prev = guess;
			guess = (guess + n / guess) / 2;
			if (guess == prev) {
				break;
			}
		}

		// The sqrt operation halves the number of bits, so we'll we'll have to calculate a shift back
		size_t unshift = fractional_bits - (shift + fractional_bits) / 2;

		return from_raw_value(guess << unshift);
	}

	constexpr double atan2(const FixedPoint &n) {
		return std::atan2(this->to_double(), n.to_double());
	}

	constexpr double sin() {
		return std::sin(this->to_double());
	}

	constexpr double cos() {
		return std::cos(this->to_double());
	}

	constexpr double tan() {
		return std::tan(this->to_double());
	}
};


// Binary operators

/**
 * FixedPoint + FixedPoint
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator+(const FixedPoint<I, F, Inter> &lhs, const FixedPoint<I, F, Inter> &rhs) {
	return FixedPoint<I, F, Inter>::from_raw_value(lhs.get_raw_value() + rhs.get_raw_value());
}

/**
 * FixedPoint + double
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator+(const FixedPoint<I, F, Inter> &lhs, const double &rhs) {
	return FixedPoint<I, F, Inter>{lhs} + FixedPoint<I, F, Inter>::from_double(rhs);
}

/**
 * FixedPoint - FixedPoint
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator-(const FixedPoint<I, F, Inter> &lhs, const FixedPoint<I, F, Inter> &rhs) {
	return FixedPoint<I, F, Inter>::from_raw_value(lhs.get_raw_value() - rhs.get_raw_value());
}

/**
 * FixedPoint - double
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator-(const FixedPoint<I, F, Inter> &lhs, const double &rhs) {
	return FixedPoint<I, F, Inter>{lhs} - FixedPoint<I, F, Inter>::from_double(rhs);
}


/**
 * FixedPoint * N
 */
template <typename I, unsigned F, typename Inter, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F, Inter>>::type constexpr operator*(const FixedPoint<I, F, Inter> lhs, const N &rhs) {
	return FixedPoint<I, F, Inter>::from_raw_value(lhs.get_raw_value() * rhs);
}

/*

*/

/**
 * FixedPoint * FixedPoint
 *
 * FixedPoint * FixedPoint can result in surprising overflows.
 *
 * using fp = FixedPoint<uint64_t, 16>;
 * fp a = fp.from_int(1 << 16);
 * => a * a will overflow because:
 *    a.rawvalue == 2^(16+16) == 2^32
 *    -> a.rawvalue * a.rawvalue == 2^64 => pwnt
 *
 * Use a larger intermediate type to prevent overflow
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator*(const FixedPoint<I, F, Inter> lhs, const FixedPoint<I, F, Inter> rhs) {
	Inter ret = static_cast<Inter>(lhs.get_raw_value()) * static_cast<Inter>(rhs.get_raw_value());
	ret >>= F;

	return FixedPoint<I, F, Inter>::from_raw_value(static_cast<I>(ret));
}


/**
 * FixedPoint / FixedPoint
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator/(const FixedPoint<I, F, Inter> lhs, const FixedPoint<I, F, Inter> rhs) {
	Inter ret = div((static_cast<Inter>(lhs.get_raw_value()) << F), static_cast<Inter>(rhs.get_raw_value()));
	return FixedPoint<I, F, Inter>::from_raw_value(static_cast<I>(ret));
}


/**
 * FixedPoint / N
 */
template <typename I, unsigned F, typename Inter, typename N>
constexpr FixedPoint<I, F, Inter> operator/(const FixedPoint<I, F, Inter> lhs, const N &rhs) {
	return FixedPoint<I, F, Inter>::from_raw_value(div(lhs.get_raw_value(), static_cast<I>(rhs)));
}

/**
 * FixedPoint % FixedPoint (modulo)
 */
template <typename I, unsigned int F, typename Inter>
constexpr FixedPoint<I, F, Inter> operator%(const FixedPoint<I, F, Inter> lhs, const FixedPoint<I, F, Inter> rhs) {
	auto div = (lhs / rhs);
	auto n = div.to_int();
	return lhs - (rhs * n);
}

} // namespace util
} // namespace openage


// std function overloads
namespace std {

template <typename I, unsigned F, typename Inter>
constexpr double sqrt(openage::util::FixedPoint<I, F, Inter> n) {
	return static_cast<double>(n.sqrt());
}

template <typename I, unsigned F, typename Inter>
constexpr double atan2(openage::util::FixedPoint<I, F, Inter> x, openage::util::FixedPoint<I, F, Inter> y) {
	return x.atan2(y);
}

template <typename I, unsigned F, typename Inter>
constexpr double sin(openage::util::FixedPoint<I, F, Inter> n) {
	return n.sin();
}

template <typename I, unsigned F, typename Inter>
constexpr double cos(openage::util::FixedPoint<I, F, Inter> n) {
	return n.cos();
}

template <typename I, unsigned F, typename Inter>
constexpr double tan(openage::util::FixedPoint<I, F, Inter> n) {
	return n.tan();
}

template <typename I, unsigned F, typename Inter>
constexpr openage::util::FixedPoint<I, F, Inter> min(openage::util::FixedPoint<I, F, Inter> x, openage::util::FixedPoint<I, F, Inter> y) {
	return openage::util::FixedPoint<I, F, Inter>::from_raw_value(
		std::min(x.get_raw_value(),
	             y.get_raw_value()));
}

template <typename I, unsigned F, typename Inter>
constexpr openage::util::FixedPoint<I, F, Inter> max(openage::util::FixedPoint<I, F, Inter> x, openage::util::FixedPoint<I, F, Inter> y) {
	return openage::util::FixedPoint<I, F, Inter>::from_raw_value(
		std::max(x.get_raw_value(),
	             y.get_raw_value()));
}

template <typename I, unsigned F, typename Inter>
constexpr openage::util::FixedPoint<I, F, Inter> abs(openage::util::FixedPoint<I, F, Inter> n) {
	return openage::util::FixedPoint<I, F, Inter>::from_raw_value(
		std::abs(n.get_raw_value()));
}

template <typename I, unsigned F, typename Inter>
constexpr double hypot(openage::util::FixedPoint<I, F, Inter> x, openage::util::FixedPoint<I, F, Inter> y) {
	return x.hypot(y);
}

template <typename I, unsigned F, typename Inter>
struct hash<openage::util::FixedPoint<I, F, Inter>> {
	constexpr size_t operator()(const openage::util::FixedPoint<I, F, Inter> &n) const {
		return std::hash<I>{}(n.raw_value);
	}
};

template <typename I, unsigned F, typename Inter>
struct numeric_limits<openage::util::FixedPoint<I, F, Inter>> {
	constexpr static openage::util::FixedPoint<I, F, Inter> min() {
		return openage::util::FixedPoint<I, F, Inter>::min_value();
	}

	constexpr static openage::util::FixedPoint<I, F, Inter> max() {
		return openage::util::FixedPoint<I, F, Inter>::max_value();
	}
};

} // namespace std
