// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "value_container.h"

namespace openage {
namespace tube {

template<typename _T>
class Discrete : public ValueContainer<_T> {
public:
	_T get(const tube_time_t &) const override;
};

template <typename _T>
_T Discrete<_T>::get(const tube_time_t &time) const {
	auto e = this->container.last(time, this->last_element);
	this->last_element = e; // TODO if Cacheing?
	return e->value;
}

}} // openage::tube
