// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <type_traits>
#include <sstream>

#include "value_container.h"
#include "../log/log.h"

namespace openage::curve {


/**
 * Continuous Datatype.
 * Stores a value container with continuous access.
 * The bound template type T has to implement `operator+(T)` and
 * `operator*(time_t)`.
 */
template<typename T>
class Continuous : public ValueContainer<T> {
public:
	using ValueContainer<T>::ValueContainer;
	/**
	 * will interpolate between the keyframes linearly based on the time.
	 */
	T get(const time_t &) const override;

	/** human readable identifier */
	std::string idstr() const override;
};



template <typename T>
T Continuous<T>::get(const time_t &time) const {
	const auto &e = this->container.last(time, this->last_element);
	this->last_element = e;

	auto nxt = e;
	++nxt;

	time_t diff_time = 0;

	auto offset = time - e->time;

	// If we do not have a next (buffer underrun!!) we assign values
	if (nxt == this->container.end()) {
		// log::log(WARN << "Continuous buffer underrun. This might be bad! Assuming constant.");
	} else {
		diff_time = nxt->time - e->time;
	}

	if (nxt == this->container.end() // use the last curve value
	    || offset == 0               // values equal -> don't need to interpolate
	    || diff_time == 0) {         // values at the same time -> division-by-zero-error

		return e->value;
	} else {
		// Interpolation between time(now) and time(next) that has elapsed
		double elapsed_frac = offset.to_double() / diff_time.to_double();

		return e->value + (nxt->value - e->value) * elapsed_frac;
	}
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


} // openage::curve
