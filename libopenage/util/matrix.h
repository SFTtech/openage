// Copyright 2015-2018 the openage authors. See copying.md for legal info.

#pragma once

#include <array>
#include <cmath>
#include <cstring>
#include <iostream>
#include <type_traits>

#include "vector.h"

namespace openage::util {

/**
 * Matrix class with arithmetic. M rows, N columns.
 * T = underlying single value type (float, double, ...)
 */
template<size_t M, size_t N, typename T>
class Matrix : public std::array<std::array<T, N>, M> {
public:
	static_assert(M > 0 and N > 0, "0-dimensional matrix not allowed");

	using this_type = Matrix<M, N, T>;

	static constexpr float default_eps = 1e-5;

	static constexpr size_t rows = M;
	static constexpr size_t cols = N;
	static constexpr bool is_square = (M == N);
	static constexpr bool is_row_vector = (M == 1);
	static constexpr bool is_column_vector = (N == 1);

	/**
	 * Initialize the matrix to zeroes.
	 */
	Matrix() {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				(*this)[i][j] = 0;
			}
		}
	}

	~Matrix() = default;

	/**
	 * Constructor from Vector
	 */
	template <bool cond=is_column_vector,
	          typename=typename std::enable_if<cond>::type>
	Matrix(const Vector<M, T> &vec) {
		for (size_t i = 0; i < M; i++) {
			(*this)[i][0] = vec[i];
		}
	}

	/**
	 * Constructor with N*M values
	 */
	template <typename ... Ts>
	Matrix(Ts ... args) {
		static_assert(sizeof...(args) == N*M, "not all values supplied");

		std::array<float, N*M> temp{{static_cast<T>(args)...}};
		for (size_t i = 0; i < N*M; i++) {
			(*this)[i / (N*M)][i % (N*M)] = temp[i];
		}
	}

	/**
	 * Constructs the identity matrix for the current size.
	 */
	template <bool cond=is_square,
	          typename=typename std::enable_if<cond>::type>
	static this_type identity() {
		this_type res;

		for (size_t i = 0; i < N; i++) {
			res[i][i] = 1;
		}

		return res;
	}

	/**
	 * Test if both matrices contain the same values within epsilon.
	 */
	bool equals(const this_type &other, float eps=default_eps) const {
		for (size_t i = 0; i < N; i++) {
			for (size_t j = 0; j < M; j++) {
				if (std::abs((*this)[i][j] - other[i][j]) >= eps) {
					return false;
				}
			}
		}
		return true;
	}

	/**
	 * Matrix multiplication
	 */
	template <size_t P>
	Matrix<M, P, T> operator *(const Matrix<N, P, T> &other) const {
		Matrix<M, P, T> res;
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
	Matrix <M, 1, T> operator *(const Vector<M, T> &vec) const {
		return (*this) * static_cast<Matrix<M, 1, T>>(vec);
	}

	/**
	 * Matrix addition
	 */
	this_type operator +(const this_type &other) const {
		this_type res;
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
	this_type operator -(const this_type &other) const {
		this_type res;
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
	void operator *=(T other) {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				(*this)[i][j] *= other;
			}
		}
	}

	/**
	 * Scalar multiplication
	 */
	this_type operator *(T other) const {
		this_type res(*this);
		res *= other;
		return res;
	}

	/**
	 * Scalar division with assignment
	 */
	void operator /=(T other) {
		for (size_t i = 0; i < M; i++) {
			for (size_t j = 0; j < N; j++) {
				(*this)[i][j] /= other;
			}
		}
	}

	/**
	 * Scalar division
	 */
	this_type operator /(T other) const {
		this_type res(*this);
		res /= other;
		return res;
	}

	/**
	 * Transposition
	 */
	Matrix<N, M, T> transpose() const {
		Matrix <N, M, T> res;
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
	         typename=typename std::enable_if<cond>::type>
	Vector<M, T> to_vector() const {
		Vector<M, T> res{};
		for (size_t i = 0; i < M; i++) {
			res[i] = (*this)[i][0];
		}
		return res;
	}

	/**
	 * Matrix trace: the sum of all diagonal entries
	 */
	template<bool cond=is_square,
	         typename=typename std::enable_if<cond>::type>
	T trace() const {
		T res = 0;

		for (size_t i = 0; i < N; i++) {
			res += (*this)[i][i];
		}

		return res;
	}

	/**
	 * Print to output stream using '<<'
	 */
	friend std::ostream &operator <<(std::ostream &o,
	                                 const this_type &mat) {
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
};

/**
 * Scalar multiplication with swapped arguments
 */
template<size_t M, size_t N, typename T>
Matrix<M, N, T> operator *(T a, const Matrix<M, N, T> &mat) {
	return mat * a;
}

/**
 * Scalar multiplication with swapped arguments for int. This was just added
 * because otherwise the above float-multiplication function might not match to
 * the template deduction.
 */
template<size_t M, size_t N, typename T>
Matrix<M, N, T> operator *(int64_t a, const Matrix<M, N, T> &mat) {
	return mat * a;
}

template<typename T=float>
using Matrix2t = Matrix<2, 2, T>;

template<typename T=float>
using Matrix3t = Matrix<3, 3, T>;

template<typename T=float>
using Matrix4t = Matrix<4, 4, T>;

template<size_t M, size_t N>
using Matrixf = Matrix<M, N, float>;

template<size_t M, size_t N>
using Matrixd = Matrix<M, N, double>;

using Matrix2f = Matrix<2, 2, float>;
using Matrix3f = Matrix<3, 3, float>;
using Matrix4f = Matrix<4, 4, float>;

using Matrix2d = Matrix<2, 2, double>;
using Matrix3d = Matrix<3, 3, double>;
using Matrix4d = Matrix<4, 4, double>;

} // openage::util
