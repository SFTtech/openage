// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include "../event/eventtarget.h"
#include "keyframe_container.h"

#include <cmath>


namespace openage::curve {

template<typename T>
class ValueContainer : public event::EventTarget {
public:
	ValueContainer(const std::shared_ptr<event::Loop> &mgr,
	               size_t id,
	               const std::string &idstr="",
	               const EventTarget::single_change_notifier &notifier=nullptr)
		:
		EventTarget(mgr, notifier),
		container{mgr},
		_id{id},
		_idstr{idstr},
		last_element{this->container.begin()} {}

	virtual ~ValueContainer() = default;

	virtual T get(const time_t &t) const = 0;

	virtual T operator ()(const time_t &now) {
		return get(now);
	}

	virtual std::pair<time_t, const T &> frame(const time_t &) const;
	virtual std::pair<time_t, const T &> next_frame(const time_t &) const;

	// Inserter mode
	virtual void set_last(const time_t &at, const T &value);
	virtual void set_insert(const time_t &at, const T &value);

	size_t id() const override {
		return this->_id;
	}

	std::string idstr() const override {
		if (this->_idstr.size() == 0) {
			return std::to_string(this->id());
		}
		return this->_idstr;
	}

protected:
	/**
	 * Stores all the keyframes
	 */
	KeyframeContainer<T> container;

	/**
	 * Identifier for the container
	 */
	const size_t _id;

	/**
	 * Human-readable identifier for the container
	 */
	const std::string _idstr;

	/**
	 * Cache the iterator for quickly finding the end
	 */
	mutable typename KeyframeContainer<T>::KeyframeIterator last_element;
};


template <typename T>
void ValueContainer<T>::set_last(const time_t &at, const T &value) {
	auto hint = this->container.last(at, this->last_element);

	// We want to remove a possible equal timed element from the container
	// to do fabs(x-y) < min is only necessary when time_t is floating point!
	//if (std::abs(hint->time - at) < std::numeric_limits<time_t>::min()) {

	if (hint->time == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	container.insert(at, value, hint);
	this->last_element = hint;

	this->changes(at);
}


template <typename T>
void ValueContainer<T>::set_insert(const time_t &at, const T &value) {
	this->container.insert(at, value, this->last_element);
	this->changes(at);
}


template <typename T>
std::pair<time_t, const T&> ValueContainer<T>::frame(const time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	return std::make_pair(e->time, e->value);
}


template <typename T>
std::pair<time_t, const T&> ValueContainer<T>::next_frame(const time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	e++;
	return std::make_pair(e->time, e->value);
}

} // openage::curve
