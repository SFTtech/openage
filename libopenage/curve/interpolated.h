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

	void compress(const time::time_t &start = time::TIME_MIN) override;

private:
	/**
	 * Get an interpolated value between two keyframes.
	 *
	 * 'before' and 'after' must be ordered such that the index of 'before' is
	 * less than the index of 'after'.
	 *
	 * @param before Index of the earlier keyframe.
	 * @param after Index of the later keyframe.
	 * @param elapsed Elapsed time after the earlier keyframe.
	 *
	 * @return Interpolated value.
	 */
	T interpolate(typename KeyframeContainer<T>::elem_ptr before,
	              typename KeyframeContainer<T>::elem_ptr after,
	              double elapsed) const;
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
	    || offset == 0                // values equal -> don't need to interpolate
	    || interval == 0) {           // values at the same time -> division-by-zero-error

		return this->container.get(e).val();
	}
	else {
		// Interpolation between time(now) and time(next) that has elapsed
		// TODO: Elapsed time does not use fixed point arithmetic
		double elapsed_frac = offset.to_double() / interval.to_double();

		return this->interpolate(e, nxt, elapsed_frac);
	}
}

template <typename T>
void Interpolated<T>::compress(const time::time_t &start) {
	// Find the last element before the start time
	auto e = this->container.last_before(start, this->last_element);

	// Store elements that should be kept
	std::vector<Keyframe<T>> to_keep;
	auto last_kept = e;
	for (auto current = e + 1; current < this->container.size() - 1; ++current) {
		// offset is between current keyframe and the last kept keyframe
		auto offset = this->container.get(current).time() - this->container.get(last_kept).time();
		auto interval = this->container.get(current + 1).time() - this->container.get(last_kept).time();
		auto elapsed_frac = offset.to_double() / interval.to_double();

		// Interpolate the value that would be at the current keyframe (if it didn't exist)
		auto interpolated = this->interpolate(last_kept, current + 1, elapsed_frac);
		if (interpolated != this->container.get(current).val()) {
			// Keep values that are different from the interpolated value
			to_keep.push_back(this->container.get(current));
			last_kept = current;
		}
	}
	// The last element is always kept, so we have to add it manually to keep it
	to_keep.push_back(this->container.get(this->container.size() - 1));

	// Erase all old keyframes after start and reinsert the non-redundant keyframes
	this->container.erase_after(e);
	for (auto elem : to_keep) {
		this->container.insert_after(elem, this->container.size() - 1);
	}

	// Update the cached element pointer
	this->last_element = e;

	// Notify observers about the changes
	this->changes(start);
}

template <typename T>
inline T Interpolated<T>::interpolate(typename KeyframeContainer<T>::elem_ptr before,
                                      typename KeyframeContainer<T>::elem_ptr after,
                                      double elapsed) const {
	ENSURE(before <= after, "Index of 'before' must be before 'after'");
	ENSURE(elapsed <= (this->container.get(after).time().to_double()
	                   - this->container.get(before).time().to_double()),
	       "Elapsed time must be less than or equal to the time between before and after");
	// TODO: after->value - before->value will produce wrong results if
	//       after->value < before->value and curve element type is unsigned
	//       Example: after = 2, before = 4; type = uint8_t ==> 2 - 4 = 254
	auto diff_value = (this->container.get(after).val() - this->container.get(before).val()) * elapsed;
	return this->container.get(before).val() + diff_value;
}


} // namespace openage::curve
