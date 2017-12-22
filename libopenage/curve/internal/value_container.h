// Copyright 2017-2017 the openage authors. See copying.md for legal info.

#pragma once

#include "../events/eventtarget.h"

#include "keyframe_container.h"

#include <cmath>

namespace openage {
namespace curve {

/**
 * pxd:
 *
 * cppclass ValueContainer(EventTarget):
 *      void set_drop(const curve_time_t &) except +
 *      void set_insert(const curve_time_t &) except +
 *
 */
template<typename _T>
class ValueContainer : public EventTarget {
public:
	ValueContainer(EventManager *mgr,
	               size_t id,
	               const EventTarget::single_change_notifier &notifier = nullptr) :
		EventTarget(mgr, notifier),
		_id{id},
		last_element{container.begin()} {}

	virtual _T get(const curve_time_t &t) const = 0;

	virtual _T operator ()(const curve_time_t &now) {
		return get(now);
	}

	virtual std::pair<curve_time_t, const _T &> frame(const curve_time_t &) const;
	virtual std::pair<curve_time_t, const _T &> next_frame(const curve_time_t &) const;

	// Inserter mode
	virtual void set_drop(const curve_time_t &at, const _T &value);
	virtual void set_insert(const curve_time_t &at, const _T &value);

	virtual size_t id() const override {
		return _id;
	}
protected:
	KeyframeContainer<_T> container;
	const size_t _id;
	mutable typename KeyframeContainer<_T>::KeyframeIterator last_element;
};


template <typename _T>
void ValueContainer<_T>::set_drop(const curve_time_t &at, const _T &value) {
	auto hint = this->container.last(at, this->last_element);

	// We want to remove a possible equal timed element from the container
    // to do fabs(x-y) < min is only necessary when curve_time_t is floating point!
    //if (std::abs(hint->time - at) < std::numeric_limits<curve_time_t>::min()) {

	if (hint->time == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	container.insert(at, value, hint);
	this->last_element = hint;

	this->on_change(at);
}


template <typename _T>
void ValueContainer<_T>::set_insert(const curve_time_t &at, const _T &value) {
	this->container.insert(at, value, this->last_element);
	this->on_change(at);
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

}} // openage::curve
