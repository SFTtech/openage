// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "simple_type.h"

namespace openage {
namespace tube {

template<typename _T>
class SimpleDiscrete : public SimpleType<_T> {
public:

	_T get() const override;
	_T get(const tube_time_t &) const override;
public:
};

template <typename _T>
_T SimpleDiscrete<_T>::get() const {
	if (this->e_now == nullptr) { // TODO THIS SUCKS!
		return _T();
	} else {
		return this->e_now->value;
	}
}

template <typename _T>
_T SimpleDiscrete<_T>::get(const tube_time_t &time) const {
	this->set_now(time);
	return this->get();
}

}} // openage::tube

