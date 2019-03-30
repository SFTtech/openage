// Copyright 2017-2019 the openage authors. See copying.md for legal info.

#pragma once

#include <sstream>

#include "keyframe_container.h"
#include "../event/evententity.h"
#include "../event/loop.h"


namespace openage::curve {

template<typename T>
class BaseCurve : public event::EventEntity {
public:
	BaseCurve(const std::shared_ptr<event::Loop> &loop,
	          size_t id,
	          const std::string &idstr="",
	          const EventEntity::single_change_notifier &notifier=nullptr)
		:
		EventEntity(loop, notifier),
		_id{id},
		_idstr{idstr},
		loop{loop},
		last_element{this->container.begin()} {}

	virtual ~BaseCurve() = default;

	virtual T get(const time_t &t) const = 0;

	virtual T operator ()(const time_t &now) {
		return get(now);
	}

	virtual std::pair<time_t, const T &> frame(const time_t &) const;
	virtual std::pair<time_t, const T &> next_frame(const time_t &) const;

	/**
	 * Insert/overwrite given value at given time and erase all elements
	 * that follow at a later time.
	 * If multiple elements exist at the given time,
	 * overwrite the last one.
	 */
	virtual void set_last(const time_t &at, const T &value);

	/**
	 * Insert a value at the given time.
	 * If there already is a value at this time,
	 * the value is inserted directly after the existing one.
	 */
	virtual void set_insert(const time_t &at, const T &value);

	/**
	 * Insert a value at the given time.
	 * If there already is a value at this time,
	 * the given value will replace the first value with the same time.
	 */
	virtual void set_replace(const time_t &at, const T &value);

	/**
	 * Remove all values that have the given time.
	 */
	virtual void erase(const time_t &at);

	/**
	 * Integrity check, for debugging/testing reasons only.
	 */
	void check_integrity() const;

	size_t id() const override {
		return this->_id;
	}

	std::string idstr() const override {
		if (this->_idstr.size() == 0) {
			return std::to_string(this->id());
		}
		return this->_idstr;
	}

	/**
	 * Get a string representation of the curve.
	 */
	std::string str() const;

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
	 * The eventloop this curve was registered to
	 */
	const std::shared_ptr<event::Loop> loop;

	/**
	 * Cache the iterator for quickly finding the end
	 */
	mutable typename KeyframeContainer<T>::iterator last_element;
};


template <typename T>
void BaseCurve<T>::set_last(const time_t &at, const T &value) {
	auto hint = this->container.last(at, this->last_element);

	// erase max one same-time value
	if (hint->time == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	this->container.insert_before(at, value, hint);
	this->last_element = hint;

	this->changes(at);
}


template <typename T>
void BaseCurve<T>::set_insert(const time_t &at, const T &value) {
	this->container.insert_after(at, value, this->last_element);
	// TODO: check if this is now the last element, then remember it
	this->changes(at);
}


template <typename T>
void BaseCurve<T>::set_replace(const time_t &at, const T &value) {
	this->container.insert_overwrite(at, value, this->last_element);
	this->changes(at);
}


template <typename T>
void BaseCurve<T>::erase(const time_t &at) {
	this->last_element = this->container.erase(at, this->last_element);
	this->changes(at);
}


template <typename T>
std::pair<time_t, const T&> BaseCurve<T>::frame(const time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	return std::make_pair(e->time, e->value);
}


template <typename T>
std::pair<time_t, const T&> BaseCurve<T>::next_frame(const time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	e++;
	return std::make_pair(e->time, e->value);
}

template <typename T>
std::string BaseCurve<T>::str() const {
	std::stringstream ss;
	ss << "Curve[" << this->idstr() << "]{" << std::endl;
	for (const auto &keyframe : this->container) {
		ss << "    " << keyframe.time << ": " << keyframe.value << "," << std::endl;
	}
	ss << "}";

	return ss.str();
}

template <typename T>
void BaseCurve<T>::check_integrity() const {
	time_t last_time = std::numeric_limits<time_t>::min();
	for (const auto &keyframe : this->container) {
		if (keyframe.time < last_time) {
			throw Error{ERR << "curve is broken after t=" << last_time << ": " << this->str()};
		}
		last_time = keyframe.time;
	}
}


} // openage::curve
