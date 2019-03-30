// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <type_traits>
#include <sstream>

#include "interpolated.h"
#include "../log/log.h"

namespace openage::curve {


/**
 * Linear curve type.
 *
 * Stores linearly interpolated line segments.
 * In contrast to `curve::Continuous`, it is not continuous and there can be jumps.
 * At one point in time, there can be multiple values. For interpolations between set values,
 * the left and rightmost one of a group of values at the same time are evaluated as fix points.
 *
 * Use the insertion operators of `ValueContainer`: `set_last`, `set_insert` and `set_replace`.
 *
 * The bound template type T has to implement `operator +(T)` and
 * `operator *(time_t)`.
 */
template<typename T>
class Segmented : public Interpolated<T> {
public:
	using Interpolated<T>::Interpolated;

	/**
	 * Insert/replace a value jump with the left and right values into the curve.
	 * The right value is used for queries at >= time,
	 * the left value for queries at < time.
	 */
	void set_insert_jump(const time_t &, const T &leftval, const T &rightval);

	/**
	 * Insert/replace a value jump with given left and right values.
	 * All following curve keyframes will be deleted, so the
	 * last two values of the curve will be `leftval` and `rightval`.
	 */
	void set_last_jump(const time_t &, const T &leftval, const T &rightval);

	/** human readable identifier */
	std::string idstr() const override;
};


template <typename T>
void Segmented<T>::set_insert_jump(const time_t &at, const T &leftval, const T &rightval) {
	auto hint = this->container.insert_overwrite(at, leftval, this->last_element, true);
	this->container.insert_after(at, rightval, hint);
	this->changes(at);
}


template <typename T>
void Segmented<T>::set_last_jump(const time_t &at, const T &leftval, const T &rightval) {
	auto hint = this->container.last(at, this->last_element);

	// erase all one same-time values
	while (hint->time == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	this->container.insert_before(at, rightval, hint);
	this->container.insert_before(at, leftval, hint);
	this->last_element = hint;

	this->changes(at);
}


template <typename T>
std::string Segmented<T>::idstr() const {
	std::stringstream ss;
	ss << "SegmentedCurve[";
	if (this->_idstr.size()) {
		ss << this->_idstr;
	}
	else {
		ss << this->id();
	}
	ss << "]";
	return ss.str();
}


} // openage::curve
