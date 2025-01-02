// Copyright 2023-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>
#include <sstream>
#include <string>
#include <type_traits>
#include <utility>

#include "curve/base_curve.h"
#include "curve/concept.h"
#include "curve/discrete.h"
#include "time/time.h"
#include "util/fixed_point.h"


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
template <KeyframeValueLike T>
class DiscreteMod : public Discrete<T> {
public:
	using Discrete<T>::Discrete;

	// Override insertion/erasure to get interval time

	void set_last(const time::time_t &at,
	              const T &value,
	              bool compress = false) override;
	void set_insert(const time::time_t &at,
	                const T &value,
	                bool compress = false) override;
	void erase(const time::time_t &at) override;

	/**
	 * Get a human readable id string.
	 */
	std::string idstr() const override;

	/**
	 * Get the raw value of the last keyframe with time <= (t - start) % interval_length.
	 */
	T get_mod(const time::time_t &t, const time::time_t &start) const;

	/**
	 * Get the last time and keyframe with time <= (t - start) % interval_length.
	 */
	std::pair<time::time_t, T> get_time_mod(const time::time_t &t, const time::time_t &start) const;

	/**
	 * Get, if existing, the time and value of keyframe with time < (t - start) % interval_length.
	 */
	std::optional<std::pair<time::time_t, T>> get_previous_mod(const time::time_t &t, const time::time_t &start) const;

private:
	/**
	 * Length of the time interval of this curve (time between first and last keyframe).
	 */
	time::time_t time_length;
};


template <KeyframeValueLike T>
void DiscreteMod<T>::set_last(const time::time_t &at,
                              const T &value,
                              bool compress) {
	BaseCurve<T>::set_last(at, value, compress);
	this->time_length = at;
}


template <KeyframeValueLike T>
void DiscreteMod<T>::set_insert(const time::time_t &at,
                                const T &value,
                                bool compress) {
	BaseCurve<T>::set_insert(at, value, compress);

	if (this->time_length < at) {
		this->time_length = at;
	}
}


template <KeyframeValueLike T>
void DiscreteMod<T>::erase(const time::time_t &at) {
	BaseCurve<T>::erase(at);

	if (this->time_length == at) {
		this->time_length = this->container.get(this->container.size() - 1).time();
	}
}


template <KeyframeValueLike T>
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


template <KeyframeValueLike T>
T DiscreteMod<T>::get_mod(const time::time_t &time, const time::time_t &start) const {
	time::time_t offset = time - start;
	if (this->time_length == 0) {
		// modulo would fail here so return early
		return Discrete<T>::get(0);
	}

	time::time_t mod = offset % this->time_length;
	return Discrete<T>::get(mod);
}


template <KeyframeValueLike T>
std::pair<time::time_t, T> DiscreteMod<T>::get_time_mod(const time::time_t &time,
                                                        const time::time_t &start) const {
	time::time_t offset = time - start;
	if (this->time_length == 0) {
		// modulo would fail here so return early
		return Discrete<T>::get_time(0);
	}

	time::time_t mod = offset % this->time_length;
	return Discrete<T>::get_time(mod);
}


template <KeyframeValueLike T>
std::optional<std::pair<time::time_t, T>> DiscreteMod<T>::get_previous_mod(const time::time_t &time,
                                                                           const time::time_t &start) const {
	time::time_t offset = time - start;
	if (this->time_length == 0) {
		// modulo would fail here so return early
		return Discrete<T>::get_previous(0);
	}

	time::time_t mod = offset % this->time_length;
	return Discrete<T>::get_previous(mod);
}

} // namespace openage::curve
