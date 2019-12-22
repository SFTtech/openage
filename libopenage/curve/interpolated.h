// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <type_traits>

#include "base_curve.h"
#include "../log/log.h"

namespace openage::curve {

/**
 * Interpolation base class.
 *
 * Extends the value container to support interpolation between values.
 *
 * The bound template type T has to implement `operator +(T)` and
 * `operator *(time_t)`.
 */
template<typename T>
class Interpolated : public BaseCurve<T> {
public:
	using BaseCurve<T>::BaseCurve;

	/**
	 * Will interpolate between the keyframes linearly based on the time.
	 * Picks two adjacent keyframes for interpolation.
	 * Uses the leftmost element of the right keyframe group with same times
	 * and the rightmost element of the left keyframe group with same times.
	 * A curve of [0:0, 1:5, 1:10, 2:20] evaluated at t=0.5 is 2.5, t=1 is 10,
	 * at t=1.5 is 15.
	 *
	 * example for a <= t <= b:
	 * val([a:x, b:y], t) = x + (y - x)/(b - a) * (t - a)
	 */

	T get(const time_t &) const override;
};



template <typename T>
T Interpolated<T>::get(const time_t &time) const {
	const auto &e = this->container.last(time, this->last_element);
	this->last_element = e;

	auto nxt = e;
	++nxt;

	time_t interval = 0;

	auto offset = time - e->time;

	if (nxt != this->container.end()) {
		interval = nxt->time - e->time;
	}

	// here, offset > interval will never hold.
	// otherwise the underlying storage is broken.

	// If the next element is at the same time, just return the value of this one.
	if (nxt == this->container.end() // use the last curve value
	    || offset == 0               // values equal -> don't need to interpolate
	    || interval == 0) {          // values at the same time -> division-by-zero-error

		return e->value;
	}
	else {
		// Interpolation between time(now) and time(next) that has elapsed
		double elapsed_frac = offset.to_double() / interval.to_double();
		return e->value + (nxt->value - e->value) * elapsed_frac;
	}
}


} // openage::curve
