// Copyright 2019-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "time/time.h"
#include "util/fixed_point.h"


namespace openage::curve {

/**
 * A element of the curvecontainer. This is especially used to keep track of
 * the value-timing.
 *
 * If you change this class, remember to update the gdb pretty printers
 * in etc/gdb_pretty/printers.py.
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
		value{value} {}

	const time::time_t &time() const {
		return this->timestamp;
	}

	const T &val() const {
		return this->value;
	}

private:
	time::time_t timestamp = time::TIME_MIN;

	T value = T{};
};

} // namespace openage::curve
