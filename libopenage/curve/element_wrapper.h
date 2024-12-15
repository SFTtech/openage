// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include "time/time.h"

namespace openage::curve {

/**
 * wrapper class for elements of a curve, store the insertion time and erase time of the element
 * aswell the data of the element.
 */
template <typename T>
struct element_wrapper {
	// Insertion time of the element
	time::time_t _alive;
	// Erase time of the element
	time::time_t _dead;
	// Element value
	T value;

	/**
	 * construct new element, set its start time and value
	 * its end time will be set to time::TIME_MAX
	 */
	element_wrapper(const time::time_t &time, const T &value) :
		_alive{time},
		_dead{time::TIME_MAX},
		value{value} {}

	// construct new element, set its start time, end time and value
	element_wrapper(const T &value, const time::time_t &alive, const time::time_t &dead) :
		_alive{alive},
		_dead{dead},
		value{value} {}


	// start time of this element
	const time::time_t &alive() const {
		return _alive;
	}

	// end time of this element
	const time::time_t &dead() const {
		return _dead;
	}

	// set the end time of this element
	void set_dead(const time::time_t &time) {
		_dead = time;
	}
};

} // namespace openage::curve
