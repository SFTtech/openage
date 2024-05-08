// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <sstream>
#include <string>

#include "curve/interpolated.h"
#include "time/time.h"


namespace openage::curve {


/**
 * Continuous linear curve.
 *
 * At one point in time, there can be only one value,
 * thus, there can't be jumps. All values are connected through linear
 * interpolation.
 *
 * The bound template type T has to implement `operator+(T)` and
 * `operator*(time::time_t)`.
 */
template <typename T>
class Continuous : public Interpolated<T> {
public:
	using Interpolated<T>::Interpolated;

	/**
	 * Insert/overwrite given value at given time and erase all elements
	 * that follow at a later time.
	 * If multiple elements exist at the given time,
	 * overwrite all of them.
	 */
	void set_last(const time::time_t &t, const T &value) override;

	/** This just calls set_replace in order to guarantee the continuity. */
	void set_insert(const time::time_t &t, const T &value) override;

	/** human readable identifier */
	std::string idstr() const override;
};


template <typename T>
void Continuous<T>::set_last(const time::time_t &at, const T &value) {
	auto hint = this->container.last(at, this->last_element);

	// erase all same-time entries
	while (this->container.get(hint).time() == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	this->container.insert_before(at, value, hint);
	this->last_element = hint;

	this->changes(at);
}


template <typename T>
void Continuous<T>::set_insert(const time::time_t &t, const T &value) {
	this->set_replace(t, value);
}


template <typename T>
std::string Continuous<T>::idstr() const {
	std::stringstream ss;
	ss << "ContinuousCurve[";
	if (this->_idstr.size()) {
		ss << this->_idstr;
	}
	else {
		ss << this->id();
	}
	ss << "]";
	return ss.str();
}


} // namespace openage::curve
