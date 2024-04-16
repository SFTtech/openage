// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "time/time.h"
#include "util/fixed_point.h"


namespace openage::curve {

/**
 * A element of the curvecontainer. This is especially used to keep track of
 * the value-timing.
 */
template <typename T>
class Keyframe {
public:
	/**
	 * New default object at numericlimits<time>::min.
	 */
	Keyframe() {}

	/**
	 * New, default-constructed element at the given time
	 */
	Keyframe(const time::time_t &time) :
		timestamp{time} {}

	/**
	 * New element fron time and value
	 */
	Keyframe(const time::time_t &time, const T &value) :
		timestamp{time},
		val{value} {}


	/**
	 * Get the time of this keyframe.
	 *
	 * @return Keyframe time.
	 */
	const time::time_t &time() const {
		return this->timestamp;
	}

	/**
	 * Get the value of this keyframe.
	 *
	 * @return Keyframe value.
	 */
	const T &value() const {
		return this->val;
	}

private:
	/**
	 * The time of this keyframe.
	 */
	time::time_t timestamp = time::TIME_MIN;

	/**
	 * The value of this keyframe.
	 */
	T val = T{};
};

} // namespace openage::curve
