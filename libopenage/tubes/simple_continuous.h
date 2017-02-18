// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "simple_type.h"

namespace openage {
namespace tube {

template<typename _T>
class SimpleContinuous : public SimpleType<_T> {
	mutable tube_time_t diff_time;
	mutable tube_time_t offset;

public:
	void set_now(const tube_time_t &t) const override;

	_T get() const override;
	_T get(const tube_time_t &) const override;

public:
};

template <typename _T>
void SimpleContinuous<_T>::set_now(const tube_time_t &t) const {
	SimpleType<_T>::set_now(t);
	if (this->e_now == nullptr) { // TODO This Sucks!
		diff_time = 0;
		offset = 1;
	} else {
		if (this->e_now->next != nullptr) {
			diff_time = this->e_now->next->time - this->e_now->time;
		} else {
			diff_time = 0;
		}
		offset = t - this->e_now->time;
	}
}

template <typename _T>
_T SimpleContinuous<_T>::get() const {
	double elapsed_frac = (double)offset / (double)diff_time ;
	if (this->e_now == nullptr) { //TODO This Sucks!
		return _T();
	} else if (this->e_now->next == nullptr || offset == 0) {
		return this->e_now->value; //If we cannot interpolate, treat as standing still (?)	
	} else {
		return this->e_now->value + (this->e_now->next->value - this->e_now->value) * elapsed_frac;
	}
}

template <typename _T>
_T SimpleContinuous<_T>::get(const tube_time_t &time) const {
	this->set_now(time);
	return get();
}


}} // openage::tube

