// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <optional>
#include <utility>
#include <sstream>

#include "base_curve.h"


namespace openage::curve {

/**
 * Does not interpolate between values. The template type does only need to
 * implement `operator=` and copy ctor.
 */
template<typename T>
class Discrete : public BaseCurve<T> {
	static_assert(std::is_copy_assignable<T>::value,
	              "Template type is not copy assignable");
	static_assert(std::is_copy_constructible<T>::value,
	              "Template type is not copy constructible");
public:
	using BaseCurve<T>::BaseCurve;

	/**
	 * Does not interpolate anything,
	 * just returns gives the raw value of the last keyframe with time <= t.
	 */
	T get(const time_t &t) const override;

	/** human readable id string */
	std::string idstr() const override;

	/**
	 * Return the last time and keyframe with time <= t.
	 */
	std::pair<time_t, T> get_time(const time_t &t) const;

	/**
	 * Return, if existing, the time and value of keyframe with time < t
	 */
	std::optional<std::pair<time_t, T>> get_previous(const time_t &t) const;
};


template <typename T>
T Discrete<T>::get(const time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e;   // TODO if Caching?
	return e->value;
}


template <typename T>
std::string Discrete<T>::idstr() const {
	std::stringstream ss;
	ss << "DiscreteCurve[";
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
std::pair<time_t, T> Discrete<T>::get_time(const time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e;
	return std::make_pair(e->time, e->value);
}


template <typename T>
std::optional<std::pair<time_t, T>> Discrete<T>::get_previous(const time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e;

	// if we're not at the container head
	// go back one entry.
	if (e == std::begin(this->container)) {
		return {};
	}

	e--;
	return std::make_pair(e->time, e->value);
}

} // openage::curve
