// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/discrete.h"


namespace openage::curve {

/**
 * Specialized discrete curve that allows wrapping simulation time values into an
 * interval defined by the first and last keyframe. This is useful for
 * data mapping to (pre-defined) consistent intervals that repeat very often, e.g.
 * animation keyframes.
 *
 * When using this curve, consider that all keyframe times should be relative to
 * the local interval time (and not simulation time), so the first keyframe should
 * always be inserted at t = 0. Also, the last keyframe should have the same value
 * as the first keyframe as a convention.
 */
template <typename T>
class DiscreteMod : public Discrete<T> {
	static_assert(std::is_copy_assignable<T>::value,
	              "Template type is not copy assignable");
	static_assert(std::is_copy_constructible<T>::value,
	              "Template type is not copy constructible");

public:
	using Discrete<T>::Discrete;

	/**
	 * Get a human readable id string.
	 */
	std::string idstr() const override;

	/**
	 * Get the raw value of the last keyframe with time <= (t - start) % interval_length.
	 */
	T get_mod(const time_t &t, const time_t &start) const;

	/**
	 * Get the last time and keyframe with time <= (t - start) % interval_length.
	 */
	std::pair<time_t, T> get_time_mod(const time_t &t, const time_t &start) const;

	/**
	 * Get, if existing, the time and value of keyframe with time < (t - start) % interval_length.
	 */
	std::optional<std::pair<time_t, T>> get_previous_mod(const time_t &t, const time_t &start) const;
};


template <typename T>
std::string DiscreteMod<T>::idstr() const {
	std::stringstream ss;
	ss << "DiscreteRingCurve[";
	if (this->_idstr.size()) {
		ss << this->_idstr;
	}
	else {
		ss << this->id();
	}
	ss << "]";
	return ss.str();
}

template <typename T>
T DiscreteMod<T>::get_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	time_t mod = offset % this->last_element->time;

	return Discrete<T>::get(mod);
}

template <typename T>
std::pair<time_t, T> DiscreteMod<T>::get_time_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	time_t mod = offset % this->last_element->time;

	return Discrete<T>::get_time(mod);
}

template <typename T>
std::optional<std::pair<time_t, T>> DiscreteMod<T>::get_previous_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	time_t mod = offset % this->last_element->time;

	return Discrete<T>::get_previous(mod);
}

} // namespace openage::curve
