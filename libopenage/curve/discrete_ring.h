// Copyright 2023-2023 the openage authors. See copying.md for legal info.

#pragma once

#include "curve/discrete.h"


namespace openage::curve {

template <typename T>
class DiscreteRing : public Discrete<T> {
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

	T get_mod(const time_t &t, const time_t &start) const;
	std::pair<time_t, T> get_time_mod(const time_t &t, const time_t &start) const;
	std::optional<std::pair<time_t, T>> get_previous_mod(const time_t &t, const time_t &start) const;

private:
	/**
     * Time between first and last keyframe.
     */
	time_t time_length;
};


template <typename T>
std::string DiscreteRing<T>::idstr() const {
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
T DiscreteRing<T>::get_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	auto div = (offset / this->time_length).to_int();
	time_t mod = offset - (offset * div);

	auto e = this->container.last(mod, this->last_element);
	this->last_element = e; // TODO if Caching?
	return e->value;
}

template <typename T>
std::pair<time_t, T> DiscreteRing<T>::get_time_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	auto div = (offset / this->time_length).to_int();
	time_t mod = offset - (offset * div);

	auto e = this->container.last(mod, this->last_element);
	this->last_element = e; // TODO if Caching?
	return std::make_pair(e->time, e->value);
}

template <typename T>
std::optional<std::pair<time_t, T>> DiscreteRing<T>::get_previous_mod(const time_t &time, const time_t &start) const {
	time_t offset = time - start;
	auto div = (offset / this->time_length).to_int();
	time_t mod = offset - (offset * div);

	auto e = this->container.last(mod, this->last_element);
	this->last_element = e; // TODO if Caching?

	// if we're not at the container head
	// go back one entry.
	if (e == std::begin(this->container)) {
		return {};
	}

	e--;
	return std::make_pair(e->time, e->value);
}


} // namespace openage::curve