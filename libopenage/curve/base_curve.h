// Copyright 2017-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <concepts>
#include <cstddef>
#include <functional>
#include <memory>
#include <sstream>
#include <string>
#include <utility>

#include "error/error.h"
#include "log/log.h"
#include "log/message.h"

#include "curve/concept.h"
#include "curve/keyframe_container.h"
#include "event/evententity.h"
#include "time/time.h"
#include "util/fixed_point.h"


namespace openage {
namespace event {
class EventLoop;
}

namespace curve {

template <KeyframeValueLike T>
class BaseCurve : public event::EventEntity {
public:
	BaseCurve(const std::shared_ptr<event::EventLoop> &loop,
	          size_t id,
	          const std::string &idstr = "",
	          const EventEntity::single_change_notifier &notifier = nullptr,
	          const T &defaultval = T()) :
		EventEntity(loop, notifier),
		container{defaultval},
		_id{id},
		_idstr{idstr},
		loop{loop},
		last_element{this->container.size()} {}

	virtual ~BaseCurve() = default;

	// prevent copying because it invalidates the usage of unique ids and event
	// registration. If you need to copy a curve, use the sync() method.
	// TODO: if copying is enabled again, these members have to be reassigned: _id, _idstr, last_element
	BaseCurve(const BaseCurve &) = delete;
	BaseCurve &operator=(const BaseCurve &) = delete;

	BaseCurve(BaseCurve &&) = default;

	virtual T get(const time::time_t &t) const = 0;

	virtual T operator()(const time::time_t &now) {
		return get(now);
	}

	/**
	 * Get the closest keyframe with t <= \p time.
	 *
	 * @return Keyframe time and value.
	 */
	virtual std::pair<time::time_t, const T> frame(const time::time_t &time) const;

	/**
	 * Get the closest keyframe with t > \p time.
	 *
	 * @return Keyframe time and value.
	 */
	virtual std::pair<time::time_t, const T> next_frame(const time::time_t &time) const;

	/**
	 * Insert/overwrite given value at given time and erase all elements
	 * that follow at a later time.
	 *
	 * If multiple elements exist at the given time,
	 * overwrite the last one.
	 *
	 * @param at Time the keyframe is inserted at.
	 * @param value Value of the keyframe.
	 * @param compress If true, only insert the keyframe if the value at time \p at
	 *                 is different from the given value.
	 */
	virtual void set_last(const time::time_t &at,
	                      const T &value,
	                      bool compress = false);

	/**
	 * Insert a value at the given time.
	 *
	 * If there already is a value at this time,
	 * the value is inserted directly after the existing one.
	 *
	 * @param at Time the keyframe is inserted at.
	 * @param value Value of the keyframe.
	 * @param compress If true, only insert the keyframe if the value at time \p at
	 *                 is different from the given value.
	 */
	virtual void set_insert(const time::time_t &at,
	                        const T &value,
	                        bool compress = false);

	/**
	 * Insert a value at the given time.
	 *
	 * If there already is a value at this time,
	 * the given value will replace the first value with the same time.
	 *
	 * @param at Time the keyframe is inserted at.
	 * @param value Value of the keyframe.
	 */
	virtual void set_replace(const time::time_t &at,
	                         const T &value);

	/**
	 * Remove all values that have the given time.
	 */
	virtual void erase(const time::time_t &at);

	/**
	 * Compress the curve by removing redundant keyframes.
	 *
	 * A keyframe is redundant if it doesn't change the value calculation of the curve
	 * at any given time, e.g. duplicate keyframes.
	 *
	 * @param start Start time at which keyframes are compressed (default = -INF).
	 *              Using the default value compresses ALL keyframes of the curve.
	 */
	virtual void compress(const time::time_t &start = time::TIME_MIN) = 0;

	/**
	 * Integrity check, for debugging/testing reasons only.
	 */
	void check_integrity() const;

	/**
	 * Copy keyframes from another curve to this curve. After syncing, the two curves
	 * are guaranteed to return the same values for t >= start.
	 *
	 * The operation may insert new keyframes at \p start on the curve.
	 *
	 * @param other Curve that keyframes are copied from.
	 * @param start Start time at which keyframes are replaced (default = -INF).
	 *              Using the default value replaces ALL keyframes of \p this with
	 *              the keyframes of \p other.
	 * @param compress If true, redundant keyframes are not copied during the sync.
	 *                 Redundant keyframes are keyframes that don't change the value
	 *                 calculaton of the curve at any given time, e.g. duplicate keyframes.
	 */
	void sync(const BaseCurve<T> &other,
	          const time::time_t &start = time::TIME_MIN,
	          bool compress = false);

	/**
	 * Copy keyframes from another curve (with a different element type) to this curve.
	 * After syncing, the two curves are guaranteed to return the same values
	 * for t >= start.
	 *
	 * The operation may insert new keyframes at \p start on the curve.
	 *
	 * @param other Curve that keyframes are copied from.
	 * @param converter Function that converts the value type of \p other to the
	 *                  value type of \p this.
	 * @param start Start time at which keyframes are replaced (default = -INF).
	 *              Using the default value replaces ALL keyframes of \p this with
	 *              the keyframes of \p other.
	 * @param compress If true, redundant keyframes are not copied during the sync.
	 *                 Redundant keyframes are keyframes that don't change the value
	 *                 calculaton of the curve at any given time, e.g. duplicate keyframes.
	 */
	template <KeyframeValueLike O>
	void sync(const BaseCurve<O> &other,
	          const std::function<T(const O &)> &converter,
	          const time::time_t &start = time::TIME_MIN,
	          bool compress = false);

	/**
	 * Get the identifier of this curve.
	 *
	 * @return Identifier.
	 */
	size_t id() const override {
		return this->_id;
	}

	/**
	 * Get the human-readable identifier of this curve.
	 *
	 * @return Human-readable identifier.
	 */
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

	/**
	 * Get the container containing all keyframes of this curve.
	 *
	 * @return Keyframe container.
	 */
	const KeyframeContainer<T> &get_container() const {
		return this->container;
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
	 * The eventloop this curve was registered to
	 */
	const std::shared_ptr<event::EventLoop> loop;

	/**
	 * Cache the index of the last accessed element (usually the end).
	 */
	mutable typename KeyframeContainer<T>::elem_ptr last_element;
};


template <KeyframeValueLike T>
void BaseCurve<T>::set_last(const time::time_t &at,
                            const T &value,
                            bool compress) {
	auto hint = this->container.last(at, this->last_element);

	// erase max one same-time value
	if (this->container.get(hint).time() == at) {
		hint--;
	}

	hint = this->container.erase_after(hint);

	if (compress and this->get(at) == value) {
		// skip insertion if the value is the same as the last one
		// erasure still happened, so we need to notify about the change
		this->changes(at);
		return;
	}

	this->container.insert_before(at, value, hint);
	this->last_element = hint;

	this->changes(at);
}


template <KeyframeValueLike T>
void BaseCurve<T>::set_insert(const time::time_t &at,
                              const T &value,
                              bool compress) {
	if (compress and this->get(at) == value) {
		// skip insertion if the value is the same as the last one
		return;
	}

	auto hint = this->container.insert_after(at, value, this->last_element);

	// check if this is now the final keyframe
	if (this->container.get(hint).time() > this->container.get(this->last_element).time()) {
		this->last_element = hint;
	}

	this->changes(at);
}


template <KeyframeValueLike T>
void BaseCurve<T>::set_replace(const time::time_t &at,
                               const T &value) {
	this->container.insert_overwrite(at, value, this->last_element);
	this->changes(at);
}


template <KeyframeValueLike T>
void BaseCurve<T>::erase(const time::time_t &at) {
	this->last_element = this->container.erase(at, this->last_element);
	this->changes(at);
}


template <KeyframeValueLike T>
std::pair<time::time_t, const T> BaseCurve<T>::frame(const time::time_t &time) const {
	auto e = this->container.last(time, this->container.size());
	auto elem = this->container.get(e);
	return elem.as_pair();
}


template <KeyframeValueLike T>
std::pair<time::time_t, const T> BaseCurve<T>::next_frame(const time::time_t &time) const {
	auto e = this->container.last(time, this->container.size());
	e++;
	auto elem = this->container.get(e);
	return elem.as_pair();
}

template <KeyframeValueLike T>
std::string BaseCurve<T>::str() const {
	std::stringstream ss;
	ss << "Curve[" << this->idstr() << "]{" << std::endl;
	for (const auto &keyframe : this->container) {
		ss << "    " << keyframe.time() << ": " << keyframe.val() << "," << std::endl;
	}
	ss << "}";

	return ss.str();
}

template <KeyframeValueLike T>
void BaseCurve<T>::check_integrity() const {
	time::time_t last_time = time::TIME_MIN;
	for (const auto &keyframe : this->container) {
		if (keyframe.time() < last_time) {
			throw Error{MSG(err) << "curve is broken after t=" << last_time << ": " << this->str()};
		}
		last_time = keyframe.time();
	}
}

template <KeyframeValueLike T>
void BaseCurve<T>::sync(const BaseCurve<T> &other,
                        const time::time_t &start,
                        bool compress) {
	// Copy keyframes between containers for t >= start
	this->last_element = this->container.sync(other.container, start);

	// Check if this->get() returns the same value as other->get() for t = start
	// If not, insert a new keyframe at start
	auto get_other = other.get(start);
	if (this->get(start) != get_other) {
		this->set_insert(start, get_other);
	}

	if (compress) {
		this->compress(start);
	}

	this->changes(start);
}


template <KeyframeValueLike T>
template <KeyframeValueLike O>
void BaseCurve<T>::sync(const BaseCurve<O> &other,
                        const std::function<T(const O &)> &converter,
                        const time::time_t &start,
                        bool compress) {
	// Copy keyframes between containers for t >= start
	this->last_element = this->container.sync(other.get_container(), converter, start);

	// Check if this->get() returns the same value as other->get() for t = start
	// If not, insert a new keyframe at start
	auto get_other = converter(other.get(start));
	if (this->get(start) != get_other) {
		this->set_insert(start, get_other);
	}

	if (compress) {
		this->compress(start);
	}

	this->changes(start);
}

} // namespace curve
} // namespace openage
