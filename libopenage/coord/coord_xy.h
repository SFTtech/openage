// Copyright 2016-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <ostream>

namespace openage {
namespace coord {

/**
 * Template class for all coordinate types that have the members x and y.
 *
 * There is a distinction between relative and absolute coordinate values
 * to disallow semantically-nonsensical origin-sensitive operations:
 *
 * absolute value + relative value -> absolute value
 * absolute value - absolute value -> relative value
 * relative value + relative value -> relative value
 * absolute value + absolute value -> undefined
 * relative value * scalar -> relative value
 * absolute value * scalar -> undefined
 *
 * This is the 'Absolute' version.
 *
 * 'Absolute' and 'Relative' are the absolute and relative types of the
 * derived class (CRTP).
 */
template<typename CoordType, typename Absolute, typename Relative>
struct CoordXYAbsolute {
	/**
	 * The member variables to store the actual data.
	 */
	CoordType x;
	CoordType y;

	/**
	 * We don't want to have a default constructor,
	 * because zero-initialization of absolute coordinates is
	 * origin-sensitive and thus non-sensical.
	 */
	CoordXYAbsolute() = delete;

	/**
	 * From-individual-values constructor
	 */
	constexpr CoordXYAbsolute(const CoordType &x, const CoordType &y) : x{x}, y{y} {}

	// copy constructor + assignment operator
	constexpr CoordXYAbsolute(const CoordXYAbsolute &other) : x{other.x}, y{other.y} {}
	constexpr CoordXYAbsolute &operator =(const CoordXYAbsolute &other) {
		this->x = other.x;
		this->y = other.y;

		return *this;
	}

	/**
	 * Manual to-relative-coordinate conversion function.
	 * You shouldn't need to use this.
	 */
	constexpr Relative as_relative() const {
		return Relative(this->x, this->y);
	}

	constexpr Absolute operator +(const Relative &other) const {
		return Absolute(this->x + other.x, this->y + other.y);
	}

	constexpr Relative operator -(const Absolute &other) const {
		return Relative(this->x - other.x, this->y - other.y);
	}

	constexpr Absolute operator -(const Relative &other) const {
		return Absolute(this->x - other.x, this->y - other.y);
	}

	constexpr Absolute &operator +=(const Relative &other) {
		*this = *this + other;
		return *this;
	}

	constexpr Absolute &operator -=(const Relative &other) {
		*this = *this - other;
		return *this;
	}

	constexpr bool operator ==(const Absolute &other) const {
		return (this->x == other.x) && (this->y == other.y);
	}

	constexpr bool operator !=(const Absolute &other) const {
		return !(*this == other);
	}
};


/**
 * Template class for all coordinate types that have the members x and y.
 * This is the 'Relative' version.
 *
 * 'Absolute' and 'Relative' are the absolute and relative types of the
 * derived class (CRTP).
 */
template<typename CoordType, typename Absolute, typename Relative>
struct CoordXYRelative {
	/**
	 * The member variables to store the actual data.
	 */
	CoordType x;
	CoordType y;

	/**
	 * Default constructor: sets the values to their defaults.
	 */
	CoordXYRelative() : x{}, y{} {}

	/**
	 * From-individual-values constructor
	 */
	constexpr CoordXYRelative(const CoordType &x, const CoordType &y) : x{x}, y{y} {}

	// copy constructor and assignment operator
	constexpr CoordXYRelative(const Relative &other) : x{other.x}, y{other.y} {}
	constexpr CoordXYRelative &operator =(const Relative &other) {
		this->x = other.x;
		this->y = other.y;
		return *this;
	}

	/**
	 * Manual to-absolute-coordinate conversion function.
	 * You shouldn't need to use this.
	 */
	constexpr Absolute as_absolute() const {
		return Absolute(this->x, this->y);
	}

	constexpr Relative operator +() const {
		return Relative(this->x, this->y);
	}

	constexpr Relative operator -() const {
		return Relative(-this->x, -this->y);
	}

	constexpr Absolute operator +(const Absolute &other) const {
		return Absolute(this->x + other.x, this->y + other.y);
	}

	constexpr Relative operator +(const Relative &other) const {
		return Relative(this->x + other.x, this->y + other.y);
	}

	constexpr Relative operator -(const Relative &other) const {
		return Relative(this->x - other.x, this->y - other.y);
	}

	template<typename ScalarType>
	constexpr Relative operator *(const ScalarType &scalar) const {
		return Relative(this->x * scalar, this->y * scalar);
	}

	template<typename ScalarType>
	constexpr Relative operator /(const ScalarType &scalar) const {
		return Relative(this->x / scalar, this->y / scalar);
	}

	constexpr Relative &operator +=(const Relative &other) {
		*this += other;
		return *this;
	}

	constexpr Relative &operator -=(const Relative &other) {
		*this -= other;
		return *this;
	}

	template<typename ScalarType>
	constexpr Relative &operator *=(const ScalarType &scalar) {
		*this *= scalar;
		return *this;
	}

	template<typename ScalarType>
	constexpr Relative &operator /=(const ScalarType &scalar) {
		*this /= scalar;
		return *this;
	}

	constexpr bool operator ==(const Relative &other) const {
		return (this->x == other.x) && (this->y == other.y);
	}

	constexpr bool operator !=(const Relative &other) const {
		return !(*this == other);
	}
};


template<typename CoordType, typename Absolute, typename Relative>
std::ostream &operator <<(std::ostream &os, CoordXYAbsolute<CoordType, Absolute, Relative> coord) {
	os << "[";
	os << "x: " << coord.x << ", ";
	os << "y: " << coord.y;
	os << "]";
	return os;
}


template<typename CoordType, typename Absolute, typename Relative>
std::ostream &operator <<(std::ostream &os, CoordXYRelative<CoordType, Absolute, Relative> coord) {
	os << "(";
	os << "x: " << coord.x << ", ";
	os << "y: " << coord.y;
	os << ")";
	return os;
}


} // namespace coord
} // namespace openage
