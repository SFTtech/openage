// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "keyframe_container.h"

namespace openage {
namespace curve {

template<typename _T>
class ValueContainer {
public:
	ValueContainer() :
		last_element{container.begin()} {}

	virtual _T get(const curve_time_t &t) const=0;

	virtual _T operator ()(const curve_time_t &now) {
		return get(now);
	}

	virtual bool needs_update(const curve_time_t &at);

	virtual std::pair<curve_time_t, const _T &> frame(const curve_time_t &) const;
	virtual std::pair<curve_time_t, const _T &> next_frame(const curve_time_t &) const;
public:
	// Inserter mode
	virtual void set_drop(const curve_time_t &at, const _T &value);
	virtual void set_insert(const curve_time_t &at, const _T &value);

protected:
	KeyframeContainer<_T> container;
	mutable typename KeyframeContainer<_T>::KeyframeIterator last_element;
};

template <typename _T>
void ValueContainer<_T>::set_drop(const curve_time_t &at, const _T &value) {
	auto hint = this->container.erase_after(this->container.last(at, this->last_element));
	container.insert(at, value, hint);
	last_element = hint;
}

template <typename _T>
void ValueContainer<_T>::set_insert(const curve_time_t &at, const _T &value) {
	this->container.insert(at, value, this->last_element);
}

template <typename _T>
std::pair<curve_time_t, const _T&> ValueContainer<_T>::frame(const curve_time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	return std::make_pair(e->time, e->value);
}

template <typename _T>
std::pair<curve_time_t, const _T&> ValueContainer<_T>::next_frame(const curve_time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	e ++;
	return std::make_pair(e->time, e->value);
}

template <typename _T>
bool ValueContainer<_T>::needs_update(const curve_time_t &at) {
	auto e = this->container.last(at, this->container.end());
	if (e->time > at || ++e == this->container.end() || e->time > at) {
		return true;
	} else {
		return false;
	}
}

}} // openage::curve
