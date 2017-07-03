// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "internal/value_container.h"

namespace openage {
namespace curve {

/**
 * Does not interpolate between values. The template type does only need to
 * implement `operator=` and copy ctor.
 */
template<typename _T>
class Discrete : public ValueContainer<_T> {
	static_assert(std::is_copy_assignable<_T>::value,
	              "Template type is not copy assignable");
	static_assert(std::is_copy_constructible<_T>::value,
	              "Template type is not copy constructible");
public:
	using ValueContainer<_T>::ValueContainer;

    /**
     * Does not interpolate anything, just gives the raw value of the keyframe
     */
	_T get(const curve_time_t &) const override;
};

template <typename _T>
_T Discrete<_T>::get(const curve_time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e; // TODO if Cacheing?
	return e->value;
}

}} // openage::curve
