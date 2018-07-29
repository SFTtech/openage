// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <type_traits>

#include "../error/error.h"
#include "../log/log.h"


namespace openage::util {

/**
 * Vector class with arithmetic.
 *
 * N = dimensions
 * T = underlying single value type (double, float, ...)
 */
template<size_t N, typename T>
class Vector : public std::array<T, N> {
public:
	static_assert(N > 0, "0-dimensional vector not allowed");

	using this_type = Vector<N, T>;

	/**
	 * Default comparison epsilon.
	 */
	static constexpr T default_eps = 1e-4;

	/**
	 * Default, random-value constructor.
	 */
	Vector() {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] = 0;
		}
	}

	~Vector() = default;

	/**
	 * Constructor for initialisation with N T values
	 */
	template<typename ... Ts>
	Vector(Ts ... args)
		:
		std::array<T, N>{static_cast<T>(args)...} {

		static_assert(sizeof...(args) == N, "not all values supplied.");
	}

	/**
	 * Equality test with given precision.
	 */
	bool equals(const this_type &other, T eps=default_eps) {
		for (size_t i = 0; i < N; i++) {
			T diff = std::abs((*this)[i] - other[i]);
			if (diff >= eps) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Vector addition with assignment
	 */
	this_type &operator +=(const this_type &other) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] += other[i];
		}
		return *this;
	}

	/**
	 * Vector addition
	 */
	this_type operator +(const this_type &other) const {
		this_type res(*this);
		res += other;
		return res;
	}

	/**
	 * Vector subtraction with assignment
	 */
	this_type &operator -=(const this_type &other) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] -= other[i];
		}
		return *this;
	}

	/**
	 * Vector subtraction
	 */
	this_type operator -(const this_type &other) const {
		this_type res(*this);
		res -= other;
		return res;
	}

	/**
	 * Scalar multiplication with assignment
	 */
	this_type &operator *=(T a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] *= a;
		}
		return *this;
	}

	/**
	 * Scalar multiplication
	 */
	this_type operator *(T a) const {
		this_type res(*this);
		res *= a;
		return res;
	}

	/**
	 * Scalar division with assignment
	 */
	this_type &operator /=(T a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] /= a;
		}
		return *this;
	}

	/**
	 * Scalar division
	 */
	this_type operator /(T a) const {
		this_type res(*this);
		res /= a;
		return res;
	}

	/**
	 * Dot product of two Vectors
	 */
	T dot(const this_type &other) const {
		T res = 0;
		for (size_t i = 0; i < N; i++) {
			res += (*this)[i] * other[i];
		}
		return res;
	}

	/**
	 * Euclidian norm aka length
	 */
	T norm() const {
		return std::sqrt(this->dot(*this));
	}

	/**
	 * Scales the Vector so that its norm is 1
	 */
	this_type &normalize() {
		*this /= this->norm();
		return *this;
	}

	/**
	 * Cross-product of two 3-dimensional vectors
	 */
	template<typename U=this_type>
	typename std::enable_if<N==3, U>::type
	/*Vector<N>*/ cross_product(const this_type &other) const {
		return this_type(
			((*this)[1] * other[2] - (*this)[2] * other[1]),
			((*this)[2] * other[0] - (*this)[0] * other[2]),
			((*this)[0] * other[1] - (*this)[1] * other[0])
		);
	}

	/**
	 * Scalar multiplication with swapped arguments
	 */
	friend this_type operator *(T a, const this_type &v) {
		return v * a;
	}

	/**
	 * Print to output stream using '<<'
	 */
	friend std::ostream &operator <<(std::ostream &o, const this_type &v) {
		o << "(";
		for (size_t i = 0; i < N-1; i++) {
			o << v[i] << ", ";
		}
		o << v[N-1] << ")";
		return o;
	}
};


template<typename T=float>
using Vector2t = Vector<2, T>;

template<typename T=float>
using Vector3t = Vector<3, T>;

template<typename T=float>
using Vector4t = Vector<4, T>;

template<size_t N>
using Vectorf = Vector<N, float>;

template<size_t N>
using Vectord = Vector<N, double>;

using Vector2f = Vector<2, float>;
using Vector3f = Vector<3, float>;
using Vector4f = Vector<4, float>;

using Vector2d = Vector<2, double>;
using Vector3d = Vector<3, double>;
using Vector4d = Vector<4, double>;

} // openage::util
