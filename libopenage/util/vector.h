// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <type_traits>

#include "../log/log.h"

namespace openage {
namespace util {

/**
 * Vector class with arithmetic.
 */
template<size_t N>
class Vector : public std::array<float, N> {
public:
	static_assert(N > 0, "0-dimensional vector not allowed");

	/**
	 * Default comparison epsilon.
	 */
	static constexpr float default_eps = 1e-4;

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
	 * Constructor for initialisation with N float values
	 */
	template<typename ... T>
	Vector(T ... args)
		:
		std::array<float, N> {{static_cast<float>(args)...}} {

		static_assert(sizeof...(args) == N, "not all values supplied.");
	}

	/**
	 * Equality test with given precision.
	 */
	bool equals(const Vector<N> &other, float eps=default_eps) {
		for (size_t i = 0; i < N; i++) {
			float diff = std::abs((*this)[i] - other[i]);
			if (diff >= eps) {
				return false;
			}
		}
		return true;
	}

	/**
	 * Vector addition with assignment
	 */
	Vector<N> &operator +=(const Vector<N> &other) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] += other[i];
		}
		return *this;
	}

	/**
	 * Vector addition
	 */
	Vector<N> operator +(const Vector<N> &other) const {
		Vector res(*this);
		res += other;
		return res;
	}

	/**
	 * Vector subtraction with assignment
	 */
	Vector<N> &operator -=(const Vector<N> &other) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] -= other[i];
		}
		return *this;
	}

	/**
	 * Vector subtraction
	 */
	Vector<N> operator -(const Vector<N> &other) const {
		Vector res(*this);
		res -= other;
		return res;
	}

	/**
	 * Scalar multiplication with assignment
	 */
	Vector<N> &operator *=(float a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] *= a;
		}
		return *this;
	}

	/**
	 * Scalar multiplication
	 */
	Vector<N> operator *(float a) const {
		Vector res(*this);
		res *= a;
		return res;
	}

	/**
	 * Scalar division with assignment
	 */
	Vector<N> &operator /=(float a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] /= a;
		}
		return *this;
	}

	/**
	 * Scalar division
	 */
	Vector<N> operator /(float a) const {
		Vector res(*this);
		res /= a;
		return res;
	}

	/**
	 * Dot product of two Vectors
	 */
	float dot(const Vector<N> &other) const {
		float res = 0;
		for (size_t i = 0; i < N; i++) {
			res += (*this)[i] * other[i];
		}
		return res;
	}

	/**
	 * Euclidian norm aka length
	 */
	float norm() const {
		return std::sqrt(this->dot(*this));
	}

	/**
	 * Scales the Vector so that its norm is 1
	 */
	Vector<N> &normalize() {
		*this /= this->norm();
		return *this;
	}

	/**
	 * Cross-product of two 3-dimensional vectors
	 */
	template<typename T=Vector<N>>
	typename std::enable_if<N==3,T>::type
	/*Vector<N>*/ cross_product(const Vector<N> &other) const {
		return Vector(
			((*this)[1] * other[2] - (*this)[2] * other[1]),
			((*this)[2] * other[0] - (*this)[0] * other[2]),
			((*this)[0] * other[1] - (*this)[1] * other[0])
		);
	}

	/**
	 * Scalar multiplication with swapped arguments
	 */
	friend Vector<N> operator *(float a, const Vector<N> &v) {
		return v * a;
	}

	/**
	 * Print to output stream using '<<'
	 */
	friend std::ostream &operator <<(std::ostream &o, const Vector<N> &v) {
		o << "(";
		for (size_t i = 0; i < N-1; i++) {
			o << v[i] << ", ";
		}
		o << v[N-1] << ")";
		return o;
	}
};


using Vector2 = Vector<2>;
using Vector3 = Vector<3>;
using Vector4 = Vector<4>;

}} // openage::util
