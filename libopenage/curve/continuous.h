// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "internal/value_container.h"

#include "../log/log.h"

namespace openage {
namespace curve {

/**
 * Continuous Datatype.
 * Stores a value container with continuous access.
 * The bound template type _T has to implement `operator+(_T)` and
 * `operator*(curve_time_t)`.
 *
 */
template<typename _T>
class Continuous : public ValueContainer<_T> {
public:
	using ValueContainer<_T>::ValueContainer;
	/**
	 * will interpolate between the keyframes linearly based on the time.
	 */
	_T get(const curve_time_t &) const override;
};



template <typename _T>
_T Continuous<_T>::get(const curve_time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e;
	auto nxt = e;
	++nxt;

	double diff_time = 0;
	double offset = time - e->time;
	// If we do not have a next (buffer underrun!!) we assign values
	if (nxt == this->container.end()) {
//		log::log(WARN << "Continuous buffer underrun. This might be bad! Assuming constant.");
	} else {
		diff_time = nxt->time - e->time;
	}

	if (nxt == this->container.end() // We do not have next - this is bad
	        || offset == 0  // we do not have an offset - this is performance
	        || diff_time == 0) { // we do not have diff - this is division-by-zero-error
		return e->value;
	} else {
		// Fraction between time(now) and time(next) that has elapsed
		double elapsed_frac = (double)offset / (double)diff_time;
		return e->value + (nxt->value - e->value) * elapsed_frac;
	}
}

}} // openage::curve
