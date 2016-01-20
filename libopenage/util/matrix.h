// Copyright 2015-2016 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_MATRIX_H_
#define OPENAGE_UTIL_MATRIX_H_

#include <iostream>
#include <cmath>
#include <array>
#include <type_traits>

#include "vector.h"

namespace openage {
namespace util {

/**
 * Matrix class with arithmetic. M rows, N columns.
 */
template<size_t M, size_t N>
class Matrix : public std::array<std::array<float, N>, M> {
public:
	static_assert(M > 0 and N > 0, "0-dimensional matrix not allowed");

	static constexpr size_t rows = M;
	static constexpr size_t cols = N;
	static constexpr bool is_square = (M == N);
	static constexpr bool is_row_vector = (M == 1);
	static constexpr bool is_column_vector = (N == 1);

	/**
	 * Default constructor
	 */
	Matrix() = default;

	/**
	 * Default destructor
	 */
	~Matrix() = default;

	/**
	 * Constructor from Vector
	 */
	template<bool cond = is_column_vector,
	         typename = typename std::enable_if<cond>::type>
	Matrix(const Vector<M> &vec) {
		for (size_t i = 0; i < M; i++) {
			(*this)[i][0] = vec[i];
		}
	}

	/**
	 * Constructor with N*M values
	 */
	template<typename ... T>
	Matrix(T ... args) {
		std::array<float, N*M> temp{{static_cast<float>(args)...}};
		for (size_t i = 0; i < N*M; i++) {
			(*this)[i / (N*M)][i % (N*M)] = temp[i];
		}
	}

	/**
	 * Matrix multiplication
	 */
	template<size_t P>
	Matrix<M, P> operator*(const Matrix<N, P> &other) const {
		Matrix<M, P> res;
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < P; j++) {
				res[i][j] = 0;
				for (size_t k = 0; k < N; k++) {
					res[i][j] += (*this)[i][k] * other[k][j];
				}
			}
		}
		return res;
	}

	/**
	 * Matrix-Vector multiplication
	 */
	Matrix <M, 1> operator*(const Vector<M> &vec) const {
		return (*this) * static_cast<Matrix<M, 1>>(vec);
	}

	/**
	 * Matrix addition
	 */
	Matrix<M, N> operator+(const Matrix<M, N> &other) const {
		Matrix<M, N> res;
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res[i][j] = (*this)[i][j] + other[i][j];
			}
		}
		return res;
	}

	/**
	 * Matrix subtraction
	 */
	Matrix<M, N> operator-(const Matrix<M, N> &other) const {
		Matrix<M, N> res;
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res[i][j] = (*this)[i][j] - other[i][j];
			}
		}
		return res;
	}

	/**
	 * Scalar multiplication with assignment
	 */
	void operator*=(float other) {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				(*this)[i][j] *= other;
			}
		}
	}

	/**
	 * Scalar multiplication
	 */
	Matrix<M, N> operator*(float other) const {
		Matrix<M, N> res(*this);
		res *= other;
		return res;
	}

	/**
	 * Scalar division with assignment
	 */
	void operator/=(float other) {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				(*this)[i][j] /= other;
			}
		}
	}

	/**
	 * Scalar division
	 */
	Matrix<M, N> operator/(float other) const {
		Matrix<M, N> res(*this);
		res /= other;
		return res;
	}

	/**
	 * Transposition
	 */
	Matrix<N, M> transpose() const {
		Matrix <N, M> res;
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				res[j][i] = (*this)[i][j];
			}
		}
		return res;
	}

	/**
	 * Conversion to Vector
	 */
	template<bool cond=is_column_vector,
	         typename = typename std::enable_if<cond>::type>
	Vector<M> to_vector() const {
		auto res = Vector<M>();
		for (size_t i = 0; i < M; i++) {
			res[i] = (*this)[i][0];
		}
		return res;
	}

};

/**
 * Scalar multiplication with swapped arguments
 */
template<size_t M, size_t N>
Matrix<M, N> operator *(float a, const Matrix<M, N> &mat) {
	return mat * a;
}

/**
 * Print to output stream using '<<'
 */
template<size_t M, size_t N>
std::ostream &operator <<(std::ostream &o, const Matrix<M, N> &mat) {
	o << "(";
	for (size_t j = 0; j < M-1; j++) {
		o << "(";
		for (size_t i = 0; i < N-1; i++) {
			o << mat[j][i] << ",\t";
		}
		o << mat[j][N-1] << ")";
		o << "," << std::endl << " ";
	}
	o << "(";
	for (size_t i = 0; i < N-1; i++) {
		o << mat[M-1][i] << ",\t";
	}
	o << mat[M-1][N-1] << "))";
	return o;
}

using Matrix2 = Matrix<2, 2>;
using Matrix3 = Matrix<3, 3>;
using Matrix4 = Matrix<4, 4>;

}} // openage::util

#endif
