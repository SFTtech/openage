// Copyright 2017-2023 the openage authors. See copying.md for legal info.

#pragma once

#include <sstream>

#include "curve/keyframe_container.h"
#include "event/evententity.h"
#include "event/loop.h"


namespace openage::curve {

template <typename T>
class BaseCurve : public event::EventEntity {
public:
	BaseCurve(const std::shared_ptr<event::Loop> &loop,
	          size_t id,
	          const std::string &idstr = "",
	          const EventEntity::single_change_notifier &notifier = nullptr,
	          const T &defaultval = T()) :
		EventEntity(loop, notifier),
		container{defaultval},
		_id{id},
		_idstr{idstr},
		loop{loop},
		last_element{this->container.begin()} {}

	virtual ~BaseCurve() = default;

	virtual T get(const time_t &t) const = 0;

	virtual T operator()(const time_t &now) {
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

	/**
     * Sync this curve with another curve of the same type by replacing all keyframes
     * >= a given time with the keyframes of \p other.
     *
     * @param other Curve that keyframes are copied from.
     * @param start Start time at which keyframes are replaced (default = -INF).
     *              Using the default value replaces ALL keyframes of \p this with
     *              the keyframes of \p other.
     */
	void sync(const BaseCurve<T> &other,
	          const time_t &start = std::numeric_limits<time_t>::min());

	/**
     * Sync this curve with another curve of a different by replacing all keyframes
     * >= a given time with the keyframes of \p other.
     *
     * @param other Curve that keyframes are copied from.
     * @param converter Function that converts the value type of \p other to the
     *                  value type of \p this.
     * @param start Start time at which keyframes are replaced (default = -INF).
     *              Using the default value replaces ALL keyframes of \p this with
     *              the keyframes of \p other.
     */
	template <typename O>
	void sync(const BaseCurve<O> &other,
	          const std::function<T(const O &)> &converter,
	          const time_t &start = std::numeric_limits<time_t>::min());

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
	const std::shared_ptr<event::Loop> loop;

	/**
	 * Cache the iterator for quickly finding the last accessed element (usually the end)
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
	auto hint = this->container.insert_after(at, value, this->last_element);
	// check if this is now the final keyframe
	if (hint->time > this->last_element->time) {
		this->last_element = hint;
	}
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
std::pair<time_t, const T &> BaseCurve<T>::frame(const time_t &time) const {
	auto e = this->container.last(time, this->container.end());
	return std::make_pair(e->time, e->value);
}


template <typename T>
std::pair<time_t, const T &> BaseCurve<T>::next_frame(const time_t &time) const {
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

template <typename T>
inline void BaseCurve<T>::sync(const BaseCurve<T> &other,
                               const time_t &start) {
	// Erase all elements in this after start time
	auto hint = this->container.last(start, this->container.end());
	hint = this->container.erase_after(hint);

	for (const auto &keyframe : other.container) {
		if (keyframe.time >= start) {
			hint = this->container.insert_after(keyframe, hint);
		}
	}
	this->last_element = hint;

	this->changes(start);
}


template <typename T>
template <typename O>
inline void BaseCurve<T>::sync(const BaseCurve<O> &other,
                               const std::function<T(const O &)> &converter,
                               const time_t &start) {
	// Erase all elements in this after start time
	auto hint = this->container.last(start, this->container.end());
	hint = this->container.erase_after(hint);

	for (const auto &keyframe : other.get_container()) {
		if (keyframe.time >= start) {
			hint = this->container.insert_after(keyframe.time, converter(keyframe.value), hint);
		}
	}
	this->last_element = hint;

	this->changes(start);
}

} // namespace openage::curve
