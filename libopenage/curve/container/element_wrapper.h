// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include "time/time.h"

namespace openage::curve {

/**
 * Wrapper for elements in a curve container.
 *
 * Stores the lifetime of the element (insertion time and erasure time) alongside the value.
 */
template <typename T>
class element_wrapper {
public:
	/**
	 * Create a new element with insertion time \p time and a given value.
	 *
	 * Erasure time is set to time::TIME_MAX, i.e. the element is alive indefinitely.
	 *
	 * @param time Insertion time of the element.
	 * @param value Element value.
	 */
	element_wrapper(const time::time_t &time, const T &value) :
		_alive{time},
		_dead{time::TIME_MAX},
		_value{value} {}

	/**
	 * Create a new element with insertion time \p alive and erasure time \p dead and a given value.
	 *
	 * @param alive Insertion time of the element.
	 * @param dead Erasure time of the element.
	 * @param value Element value.
	 */
	element_wrapper(const time::time_t &alive, const time::time_t &dead, const T &value) :
		_alive{alive},
		_dead{dead},
		_value{value} {}

	/**
	 * Get the insertion time of this element.
	 *
	 * @return Time when the element was inserted into the container.
	 */
	const time::time_t &alive() const {
		return _alive;
	}

	/**
	 * Get the erasure time of this element.
	 *
	 * @return Time when the element was erased from the container.
	 */
	const time::time_t &dead() const {
		return _dead;
	}

	/**
	 * Set the erasure time of this element.
	 *
	 * @param time Time when the element was erased from the container.
	 */
	void set_dead(const time::time_t &time) {
		_dead = time;
	}

	/**
	 * Get the value of this element.
	 *
	 * @return Value of the element.
	 */
	const T &value() const {
		return _value;
	}

private:
	/**
	 * Time of insertion of the element into the container
	 */
	time::time_t _alive;

	/**
	 * Time of erasure of the element from the container
	 */
	time::time_t _dead;

	/**
	 * Element value
	 */
	T _value;
};

} // namespace openage::curve
