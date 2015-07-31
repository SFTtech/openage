// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_COORD_FIXED_POINT_H_
#define OPENAGE_COORD_FIXED_POINT_H_

#include <cmath>
#include <functional>

#include "../util/misc.h"

namespace openage {
namespace coord {

template<typename int_type, unsigned fractional_bits>
struct FixedPoint {

	int_type data;

	static const int_type one = int_type(1) << fractional_bits;

	static const int_type fraction_bitmask = one - 1;


	// constructors
	constexpr FixedPoint() : data(0) {
	}

	constexpr FixedPoint(int_type n) : data(n << fractional_bits) {
	}

	constexpr FixedPoint(int n) : data(n << fractional_bits) {
	}

	constexpr FixedPoint(unsigned int n) : data(n << fractional_bits) {
	}

	// Constructing FixedPoint numbers with values like 1/2 and 1/8 won't lose precision
	constexpr FixedPoint(float n) : data(static_cast<int_type>(n * one)) {
	}

	constexpr FixedPoint(double n) : data(static_cast<int_type>(n * one)) {
	}

	static FixedPoint from_raw(int_type data) {
		FixedPoint result;
		result.data = data;
		return result;
	}

	constexpr FixedPoint(const FixedPoint &o) : data(o.data) {
	}

	FixedPoint &operator=(const FixedPoint &o) {
		data = o.data;
		return *this;
	}


	// Comparison operators
	bool operator==(const FixedPoint &o) const {
		return data == o.data;
	}

	bool operator!=(const FixedPoint &o) const {
		return data != o.data;
	}

	bool operator<(const FixedPoint &o) const {
		return data < o.data;
	}

	bool operator>(const FixedPoint &o) const {
		return data > o.data;
	}

	bool operator<=(const FixedPoint &o) const {
		return data <= o.data;
	}

	bool operator>=(const FixedPoint &o) const {
		return data >= o.data;
	}

	// Unary operators
	bool operator!() const {
		return !data;
	}

	FixedPoint operator+() const {
		return *this;
	}

	FixedPoint operator-() const {
		FixedPoint f(*this);
		f.data = -f.data;
		return f;
	}


	// Basic operators
	FixedPoint &operator+=(const FixedPoint &n) {
		data += n.data;
		return *this;
	}

	FixedPoint &operator-=(const FixedPoint &n) {
		data -= n.data;
		return *this;
	}

	FixedPoint &operator*=(const FixedPoint &n) {
		*this = *this * n;
		return *this;
	}

	FixedPoint &operator/=(const FixedPoint &n) {
		*this = *this / n;
		return *this;
	}


	// Conversions
	int to_int() const {
		return (data >> fractional_bits);
	}

	float to_float() const {
		return static_cast<float>(data) / FixedPoint::one;
	}

	float to_double() const {
		return static_cast<float>(data) / FixedPoint::one;
	}

	FixedPoint get_fraction() const {
		return from_raw(data & fraction_bitmask);
	}

	int_type to_raw() const {
		return data;
	}

	explicit operator float() const {
		return to_float();
	}

	explicit operator double() const {
		return to_double();
	}

	explicit operator int() const {
		return to_int();
	}

	void swap(FixedPoint &rhs) {
		std::swap(data, rhs.data);
	}
};


// Binary operators
template<typename I, unsigned F>
FixedPoint<I, F> operator +(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>::from_raw(lhs.data + rhs.data);
}

template<typename I, unsigned F>
FixedPoint<I, F> operator -(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>::from_raw(lhs.data - rhs.data);
}

template<typename I, unsigned F>
FixedPoint<I, F> operator *(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> rhs) {
	auto n = lhs.data * rhs.data;
	return FixedPoint<I, F>::from_raw(n >> F);
}

template<typename I, unsigned F>
FixedPoint<I, F> operator /(const FixedPoint<I, F> &lhs, const FixedPoint<I, F> rhs) {
	auto n = lhs.data << F;
	return FixedPoint<I, F>::from_raw(util::div(n, rhs.data));
}


// N `op` FixedPoint
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator +(const N &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>{ lhs } + rhs;
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator -(const N &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>{ lhs } - rhs;
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator *(const N &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>{ lhs } * rhs;
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator /(const N &lhs, const FixedPoint<I, F> rhs) {
	return FixedPoint<I, F>{ lhs } / rhs;
}

// FixedPoint `op` N
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator +(const FixedPoint<I, F> lhs, const N &rhs) {
	return lhs + FixedPoint<I, F>{ rhs };
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator -(const FixedPoint<I, F> lhs, const N &rhs) {
	return lhs - FixedPoint<I, F>{ rhs };
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator *(const FixedPoint<I, F> lhs, const N &rhs) {
	return lhs * FixedPoint<I, F>{ rhs };
}
template<typename I, unsigned F, typename N>
typename std::enable_if<std::is_arithmetic<N>::value, FixedPoint<I, F>>::type
operator /(const FixedPoint<I, F> lhs, const N &rhs) {
	return lhs / FixedPoint<I, F>{ rhs };
}


// I/O operators
template<typename I, unsigned F>
std::ostream &operator <<(std::ostream &os, FixedPoint<I, F> n) {
	os << n.data;
	return os;
}

template<typename I, unsigned F>
std::istream &operator >>(std::istream &is, FixedPoint<I, F> n) {
	is >> n.data;
	return is;
}

} // namespace coord
} // namespace openage


// std function overloads
namespace std {

using openage::coord::FixedPoint;

template<typename I, unsigned F>
FixedPoint<I, F> sqrt(FixedPoint<I, F> n) {
	return FixedPoint<I, F> { std::sqrt(n.to_double()) };
}

template<typename I, unsigned F>
FixedPoint<I, F> min(FixedPoint<I, F> x, FixedPoint<I, F> y) {
	return FixedPoint<I, F> { std::min(x.data, y.data) };
}

template<typename I, unsigned F>
FixedPoint<I, F> max(FixedPoint<I, F> x, FixedPoint<I, F> y) {
	return FixedPoint<I, F> { std::max(x.data, y.data) };
}

template<typename I, unsigned F>
FixedPoint<I, F> abs(FixedPoint<I, F> n) {
	return FixedPoint<I, F> { std::abs(n.data) };
}

template<typename I, unsigned F>
FixedPoint<I, F> hypot(FixedPoint<I, F> x, FixedPoint<I, F> y) {
	return FixedPoint<I, F> { std::hypot(x.to_double(), y.to_double()) };
}

template<typename I, unsigned F>
struct hash<openage::coord::FixedPoint<I, F>> {
	size_t operator ()(const openage::coord::FixedPoint<I, F> &n) const {
		return std::hash<I>{}(n.data);
	}
};

} // namespace std

#endif
