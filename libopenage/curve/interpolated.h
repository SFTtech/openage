// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/base_curve.h"
#include "time/time.h"
#include "util/fixed_point.h"


namespace openage::curve {

/**
 * Interpolation base class.
 *
 * Extends the value container to support interpolation between values.
 *
 * The bound template type T has to implement `operator +(T)` and
 * `operator *(time::time_t)`.
 */
template <typename T>
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

	T get(const time::time_t &) const override;
};


template <typename T>
T Interpolated<T>::get(const time::time_t &time) const {
	const auto e = this->container.last(time, this->last_element);
	this->last_element = e;

	auto nxt = e;
	++nxt;

	time::time_t interval = 0;

	auto offset = time - this->container.get(e).time();

	if (nxt != this->container.size()) {
		interval = this->container.get(nxt).time() - this->container.get(e).time();
	}

	// here, offset > interval will never hold.
	// otherwise the underlying storage is broken.

	// If the next element is at the same time, just return the value of this one.
	if (nxt == this->container.size() // use the last curve value
	    || offset == 0 // values equal -> don't need to interpolate
	    || interval == 0) { // values at the same time -> division-by-zero-error

		return this->container.get(e).val();
	}
	else {
		// Interpolation between time(now) and time(next) that has elapsed
		// TODO: Elapsed time does not use fixed point arithmetic
		double elapsed_frac = offset.to_double() / interval.to_double();

		// TODO: nxt->value - e->value will produce wrong results if
		//       the nxt->value < e->value and curve element type is unsigned
		//       Example: nxt = 2, e = 4; type = uint8_t ==> 2 - 4 = 254
		auto diff_value = (this->container.get(nxt).val() - this->container.get(e).val()) * elapsed_frac;
		return this->container.get(e).val() + diff_value;
	}
}


} // namespace openage::curve
