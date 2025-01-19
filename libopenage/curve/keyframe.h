// Copyright 2019-2025 the openage authors. See copying.md for legal info.

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
		value{value},
		timestamp{time} {}

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
	const T &val() const {
		return this->value;
	}

	/**
	 * Get a time-value pair of this keyframe.
	 *
	 * @return Keyframe time-value pair.
	 */
	std::pair<time::time_t, T> as_pair() const {
		return {this->timestamp, this->value};
	}

public:
	/**
	 * Value of the keyframe.
	 *
	 * Can be modified by the curve if necessary.
	 */
	T value = T{};

private:
	/**
	 * Time of the keyframe.
	 */
	time::time_t timestamp = time::TIME_MIN;
};

} // namespace openage::curve
