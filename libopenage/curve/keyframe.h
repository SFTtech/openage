// Copyright 2019-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <limits>

#include "curve.h"

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
	Keyframe(const time_t &time) :
		time{time} {}

	/**
	 * New element fron time and value
	 */
	Keyframe(const time_t &time, const T &value) :
		time{time},
		value{value} {}

	const time_t time = std::numeric_limits<time_t>::min();
	T value = T{};
};

} // openage::curve
