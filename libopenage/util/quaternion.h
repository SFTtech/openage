// Copyright 2015-2015 the openage authors. See copying.md for legal info.

#ifndef OPENAGE_UTIL_QUATERNION_H_
#define OPENAGE_UTIL_QUATERNION_H_

#include <cmath>

#include "matrix.h"
#include "math_constants.h"
#include "vector.h"

#include "../error/error.h"
#include "../log/log.h"

namespace openage {
namespace util {

/**
 * Implements Quaternions to represent 3d rotations.
 *
 * The 4 components stores 3 components as rotation axis,
 * and one component as the rotation amount.
 *
 * This is mainly Ken Shoemake stuff from:
 * http://www.cs.ucr.edu/~vbz/resources/quatut.pdf
 *
 * Also:
 * From Quaternion to Matrix and Back
 * J.M.P. van Waveren, id Software, 2005
 */
template <typename T=float>
class Quaternion {
public:
	using this_type = Quaternion<T>;
	using type = T;

	static constexpr T default_eps = 1e-4;

	Quaternion(T w, T x, T y, T z)
		:
		w{w}, x{x}, y{y}, z{z} {}

	/**
	 * Create a identity quaternion.
	 */
	Quaternion()
		:
		w{1}, x{0}, y{0}, z{0} {}

	/**
	 * Constructs a quaternion from a rotation matrix.
	 * mat is assumed to be a left-matrix:
	 * vec_transformed = mat * vec_orig.
	 *
	 * mat can be 3x3 or 4x4.
	 *
	 * This tries to avoid float-fuckups in near-zero divides
	 * by using larger components first: w, then x, y, or z.
	 *
	 * trace(mat) >= 0 => |w| > 1/2
	 * => as small as a largest component can be.
	 * else:
	 * max diagonal entry <=> max (|x|, |y|, |z|)
	 * which is larger than |w| and >= 1/2
	 */
	template <size_t N=4>
	Quaternion(const Matrix<N, N> &mat) {

		static_assert(N == 3 or N == 4, "only 3 and 4 dimensional matrices can be converted to a quaternion!");

		T trace = mat.trace();
		T trace_cmp = trace;

		if (N == 4) {
			trace_cmp -= mat[3][3];
		} else {
			trace += 1.0;
		}

		if (trace_cmp > 0) {
			T trace_root = std::sqrt(trace); // = 2w
			this->w = trace_root * 0.5;      // = w
			trace_root = 0.5 / trace_root;   // = 1/4w

			this->x = (mat[2][1] - mat[1][2]) * trace_root;
			this->y = (mat[0][2] - mat[2][0]) * trace_root;
			this->z = (mat[1][0] - mat[0][1]) * trace_root;
		}
		else {
			// determine highest diagonal component:
			int n0 = 0;

			if (mat[1][1] > mat[0][0]) {
				n0 = 1;
			}

			if (mat[2][2] > mat[n0][n0]) {
				n0 = 2;
			}

			// following indices to 0, 1, 2
			int next_index[] = {1, 2, 0};

			// indexable pointers
			T *ptrs[] = {&this->x, &this->y, &this->z};

			int n1 = next_index[n0];
			int n2 = next_index[n1];

			// this avoids float fuckups:
			T trace_ordered = (mat[n0][n0] - (mat[n1][n1] + mat[n2][n2]));

			if (N == 4) {
				trace_ordered += mat[3][3];
			} else {
				trace_ordered += 1.0;
			}

			T trace_root = std::sqrt(trace_ordered);

			*ptrs[n0] = trace_root * 0.5;   // = w
			trace_root = 0.5 / trace_root;  // = 1/4w

			*ptrs[n1] = (mat[n0][n1] + mat[n1][n0]) * trace_root;
			*ptrs[n2] = (mat[n2][n0] + mat[n0][n2]) * trace_root;

			this->w = (mat[n2][n1] - mat[n1][n2]) * trace_root;
		}

		if (N == 4) {
			// normalize the quaternion by the matrix[3][3] entry
			(*this) *= 1.0 / std::sqrt(mat[3][3]);
		}
	}

	Quaternion(const this_type &other) = default;
	Quaternion(this_type &&other) = default;
	Quaternion &operator =(const this_type &other) = default;
	Quaternion &operator =(this_type &&other) = default;

	virtual ~Quaternion() = default;

	/**
	 * Create a quaternion from a rotation in radians around a given axis.
	 */
	static this_type from_rad(T rad, Vector3 axis) {
		T rot = rad / 2.0;
		this_type q{
			std::cos(rot),
			axis[0] * std::sin(rot),
			axis[1] * std::sin(rot),
			axis[2] * std::sin(rot)
		};
		return q;
	}

	/**
	 * Create a quaternion from a rotation in degree around a given axis.
	 */
	static this_type from_deg(T deg, Vector3 axis) {
		return this_type::from_rad((deg * math::PI) / 180.0, axis);
	}

	/**
	 * Perform a dot product with another quaternion.
	 */
	T dot(const this_type &o) const {
		return this->w * o.w + this->x * o.x + this->y * o.y + this->z * o.z;
	}

	/**
	 * Calculate the length of the quaternion.
	 */
	T norm() const {
		return std::sqrt(this->dot(*this));
	}

	/**
	 * Ensure that the quaternion's length equals 1.
	 */
	T normalize() {
		T len = this->norm();
		*this /= len;
		return len;
	}

	/**
	 * Return the normalized version of this quaternion.
	 */
	this_type normalized() const {
		this_type q{*this};
		q.normalize();
		return q;
	}

	/**
	 * Inverted the quaternion:
	 * Flip the rotation axes and scale by
	 * inverse sum of all components squared.
	 *
	 * inv(q)= conj(q)/(w*w + x*x + y*y + z*z)
	 */
	void inverse() {
		T dot = this->dot(*this);
		if (dot > 0.0) {
			*this = this->conjugated() / dot;
		}
		else {
			throw Error(MSG(err) << "tried inverting " << *this);
		}
	}

	/**
	 * Return the inverted quaternion.
	 */
	this_type inversed() const {
		this_type q{*this};
		q.inverse();
		return q;
	}

	/**
	 * Conjugate the quaternion by additive inverting
	 * the x, y and z components.
	 */
	void conjugate() {
		this->x = -this->x;
		this->y = -this->y;
		this->z = -this->z;
	}

	/**
	 * Return the conjugated quaternion.
	 */
	this_type conjugated() const {
		this_type q{*this};
		q.conjugate();
		return q;
	}

	/**
	 * Test if the rotation of both quaternions is the same.
	 */
	bool equals(const this_type &other, T eps=default_eps) const {
		T ori = this->dot(other);
		return (1 - (ori*ori)) < eps;
	}

	/**
	 * Test if both quaternion store the same numbers.
	 */
	bool equals_number(const this_type &other, T eps=default_eps) const {
		bool result = true;
		(this->w - other.w) < eps or (result = false);
		(this->x - other.x) < eps or (result = false);
		(this->y - other.y) < eps or (result = false);
		(this->z - other.z) < eps or (result = false);
		return result;
	}

	/**
	 * Test rotation equality with another quaternion
	 * with given precision in radians.
	 */
	bool equals_rad(const this_type &other, T rad_eps=default_eps) const {
		T ori = this->dot(other);
		T angle = std::acos((2.0 * (ori * ori)) - 1.0);

		return std::abs(angle) < rad_eps;
	}

	/**
	 * Test rotation equality with another quaternion
	 * with given precision in degree.
	 */
	bool equals_deg(const this_type &other, T deg_eps=default_eps) const {
		return this->equals_rad(other, (deg_eps * 180.0) / math::PI);
	}

	/**
	 * Generate the corresponding rotation matrix.
	 */
	Matrix3 to_matrix() const {
		T x2  = this->x * 2;
		T y2  = this->y * 2;
		T z2  = this->z * 2;

		T x2w = x2 * this->w;
		T y2w = y2 * this->w;
		T z2w = z2 * this->w;

		T x3  = x2 * this->x;
		T y2x = y2 * this->x;
		T z2x = z2 * this->x;

		T y3  = y2 * this->y;
		T z2y = z2 * this->y;
		T z3  = z2 * this->z;

		Matrix3 m{
			1.0 - (y3 + z3), y2x - z2w, z2x + y2w,
			y2x + z2w, 1.0 - (x3 + z3), z2y - x2w,
			z2x - y2w, z2y + x2w, 1.0 - (x3 + y3)
		};

		return m;
	}

	/**
	 * Transforms a vector by this quaternion.
	 */
	Vector3 operator *(const Vector3 &vec) const {
		Vector3 axis{this->x, this->y, this->z};

		Vector3 axis_vec_normal = axis.cross_product(vec);
		Vector3 axis_vec_inplane = axis.cross_product(axis_vec_normal);

		axis_vec_normal *= 2.0f * this->w;
		axis_vec_inplane *= 2.0f;

		return vec + axis_vec_normal + axis_vec_inplane;
	}

	const this_type &operator +=(const this_type &other) {
		this->w += other.w;
		this->x += other.x;
		this->y += other.y;
		this->z += other.z;

		return *this;
	}

	this_type operator +(const this_type &other) const {
		this_type q{*this};
		q += other;
		return q;
	}

	const this_type &operator -=(const this_type &other) {
		this->w -= other.w;
		this->x -= other.x;
		this->y -= other.y;
		this->z -= other.z;

		return *this;
	}

	this_type operator -(const this_type &other) const {
		this_type q{*this};
		q -= other;
		return q;
	}

	const this_type &operator *=(const T &fac) {
		this->w *= fac;
		this->x *= fac;
		this->y *= fac;
		this->z *= fac;

		return *this;
	}

	this_type operator *(const T &fac) const {
		this_type q{*this};
		q *= fac;
		return q;
	}

	const this_type &operator *=(const this_type &other) {
		T w_new = (this->w * other.w - this->x * other.x -
		           this->y * other.y - this->z * other.z);
		T x_new = (this->w * other.x + this->x * other.w +
		           this->y * other.z - this->z * other.y);
		T y_new = (this->w * other.y - this->x * other.z +
		           this->y * other.w + this->z * other.x);
		T z_new = (this->w * other.z + this->x * other.y -
		           this->y * other.x + this->z * other.w);

		this->w = w_new;
		this->x = x_new;
		this->y = y_new;
		this->z = z_new;

		return *this;
	}

	this_type operator *(const this_type &other) const {
		this_type q{*this};
		q *= other;
		return q;
	}

	const this_type &operator /=(const T &fac) {
		this->w /= fac;
		this->x /= fac;
		this->y /= fac;
		this->z /= fac;

		return *this;
	}

	this_type operator /(const T &fac) const {
		this_type q{*this};
		q /= fac;
		return q;
	}

	const this_type operator -() const {
		return this_type{-this->w, -this->x, -this->y, -this->z};
	}

	bool operator ==(const this_type &other) const {
		return ((this->w == other.w) and
		        (this->x == other.x) and
		        (this->y == other.y) and
		        (this->z == other.z));
	}

	bool operator !=(const this_type &other) const {
		return not (*this == other);
	}

	friend std::ostream &operator <<(std::ostream &o, const this_type &q) {
		o << "Quaternion(" << q.w << ", " << q.x;
		o << ", " << q.y << ", " << q.z << ")";
		return o;
	}

protected:
	/**
	 * Stored quaternion number components.
	 */
	T w, x, y, z;
};

}} // openage::util

#endif
