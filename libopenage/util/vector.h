// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#pragma once

#include <iostream>
#include <cmath>
#include <array>
#include <type_traits>

#include "hash.h"

namespace openage {
namespace util {

/**
 * Vector class with arithmetic.
 */
template<size_t N, typename DT=float>
class Vector : public std::array<DT, N> {
public:
	static_assert(N > 0, "0-dimensional vector not allowed");

	using data_type = DT;
	using this_type = Vector<N, data_type>;

	/**
	 * Default constructor
	 */
	Vector() = default;

	/**
	 * Default destructor
	 */
	~Vector() = default;

	/**
	 * Constructor for initialisation with N data_type values
	 */
	template<typename ... T>
	Vector(T ... args)
	:
	std::array<data_type, N> {{static_cast<data_type>(args)...}} {}

	/**
	 * Vector comparison
	 */
	typename std::enable_if<std::is_integral<data_type, typename bool>>::type
	/* bool */ operator ==(this_type &other) {
		for (size_t i = 0; i < N; i++) {
			if ((*this)[i] != other[i]) {
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
		Vector res(*this);
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
		Vector res(*this);
		res -= other;
		return res;
	}

	/**
	 * Scalar multiplication with assignment
	 */
	this_type &operator *=(data_type a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] *= a;
		}
		return *this;
	}

	/**
	 * Scalar multiplication
	 */
	this_type operator *(data_type a) const {
		Vector res(*this);
		res *= a;
		return res;
	}

	/**
	 * Scalar division with assignment
	 */
	this_type &operator /=(data_type a) {
		for (size_t i = 0; i < N; i++) {
			(*this)[i] /= a;
		}
		return *this;
	}

	/**
	 * Scalar division
	 */
	this_type operator /(data_type a) const {
		Vector res(*this);
		res /= a;
		return res;
	}

	/**
	 * Dot product of two Vectors
	 */
	data_type dot_product(const this_type &other) const {
		data_type res = 0;
		for (size_t i = 0; i < N; i++) {
			res += (*this)[i] * other[i];
		}
		return res;
	}

	/**
	 * Euclidian norm aka length
	 */
	float norm() const {
		return sqrtf((*this).dot_product(*this));
	}

	/**
	 * Scales the Vector so that its norm is 1
	 */
	template<typename T=this_type>
	typename std::enable_if<std::is_floating_point<data_type>::value, T>::type
	/* this_type */ &normalize() {
		*this /= this->norm();
		return *this;
	}

	/**
	 * Cross-product of two 3-dimensional vectors
	 */
	template<typename T=this_type>
	typename std::enable_if<N==3,T>::type
	/*this_type*/ cross_product(const this_type &other) const {
		return Vector(
			((*this)[1] * other[2] - (*this)[2] * other[1]),
			((*this)[2] * other[0] - (*this)[0] * other[2]),
			((*this)[0] * other[1] - (*this)[1] * other[0])
		);
	}

};

/**
 * Scalar multiplication with swapped arguments
 */
template<size_t N, typename mul_type, typename data_type>
Vector<N, data_type> operator *(mul_type a, const Vector<N, data_type> &v) {
	return v * a;
}

/**
 * Print to output stream using '<<'
 */
template<size_t N, typename data_type>
std::ostream &operator <<(std::ostream &o, const Vector<N, data_type> &v) {
	o << "(";
	for (size_t i = 0; i < N-1; i++) {
		o << v[i] << ", ";
	}
	o << v[N-1] << ")";
	return o;
}


using Vector2 = Vector<2>;
using Vector3 = Vector<3>;
using Vector4 = Vector<4>;

}} // openage::util


namespace std {

template<size_t N, typename DT>
struct hash<openage::util::Vector<N, DT>> {
	size_t operator ()(const openage::util::Vector<N, DT> &arg) const {
		size_t ret = 0;
		for (size_t i = 0; i < N; i++) {
			ret = openage::util::hash_combine(ret, hash<DT>{}(arg[i]));
		}
		return ret;
	}
};

} // std
