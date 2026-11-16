// Copyright 2026+ the openage authors. See copying.md for legal info.

#pragma once

#include <cstddef>
#include <memory>
#include <string>
#include <utility>
#include <vector>

#include "curve/keyframe.h"
#include "curve/keyframe_container.h"
#include "error/error.h"
#include "event/evententity.h"
#include "time/time.h"


namespace openage {
namespace event {
class EventLoop;
} // namespace event

namespace curve {

/**
 * Curve container storing a sequence of elements whose length is set at runtime.
 *
 * Each element is tracked individually over time by its own keyframe container.
 * The number of elements is fixed after construction.
 */
template <typename T>
class Vector : event::EventEntity {
public:
	/// Underlying container type.
	using container_t = std::vector<KeyframeContainer<T>>;

	/// Index type to access elements in the container.
	using index_t = typename container_t::size_type;

	/**
	 * Create a new vector curve container.
	 *
	 * @param loop Event loop this curve is registered on for notifications.
	 * @param id Unique identifier for this curve.
	 * @param default_vals Values of the elements at time -INF. Determines the number of elements.
	 * @param idstr Human-readable identifier for this curve.
	 * @param notifier Function to call when this curve changes.
	 */
	Vector(const std::shared_ptr<event::EventLoop> &loop,
	       size_t id,
	       const std::vector<T> &default_vals,
	       const std::string &idstr = "",
	       const EventEntity::single_change_notifier &notifier = nullptr);

	// prevent copying because it invalidates the usage of unique ids and event
	// registration. If you need to copy a curve, use the sync() method.
	Vector(const Vector &) = delete;
	Vector &operator=(const Vector &) = delete;

	Vector(Vector &&) = default;

	/**
	 * Get the value of an element at time t.
	 *
	 * @param t Time of access.
	 * @param index Index of the element.
	 *
	 * @return Value of the last keyframe with time <= t.
	 */
	T at(const time::time_t &t, index_t index) const;

	/**
	 * Get the values of all elements at time t.
	 *
	 * @param t Time of access.
	 *
	 * @return Values of the last keyframes with time <= t.
	 */
	std::vector<T> get(const time::time_t &t) const;

	/**
	 * Get the number of elements.
	 *
	 * @return Number of elements.
	 */
	size_t size() const;

	/**
	 * Get the last keyframe of an element with time <= t.
	 *
	 * @param t Time of access.
	 * @param index Index of the element.
	 *
	 * @return Time-value pair of the last keyframe with time <= t.
	 */
	std::pair<time::time_t, T> frame(const time::time_t &t, index_t index) const;

	/**
	 * Get the first keyframe of an element with time > t.
	 *
	 * Throws if there is no keyframe with time > t.
	 *
	 * @param t Time of access.
	 * @param index Index of the element.
	 *
	 * @return Time-value pair of the first keyframe with time > t.
	 */
	std::pair<time::time_t, T> next_frame(const time::time_t &t, index_t index) const;

	/**
	 * Insert a new keyframe for an element at time t.
	 *
	 * If the element already has keyframes at time t, the new keyframe is inserted after them.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the element.
	 * @param value Keyframe value.
	 */
	void set_insert(const time::time_t &t, index_t index, const T &value);

	/**
	 * Insert a new keyframe for an element at time t and erase all its keyframes with time > t.
	 *
	 * If the element already has keyframes at time t, the last of them is replaced.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the element.
	 * @param value Keyframe value.
	 */
	void set_last(const time::time_t &t, index_t index, const T &value);

	/**
	 * Insert a new keyframe for an element at time t, replacing its last keyframe with time == t.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the element.
	 * @param value Keyframe value.
	 */
	void set_replace(const time::time_t &t, index_t index, const T &value);

	/**
	 * Copy keyframes from another vector curve to this one.
	 *
	 * Replaces all keyframes with time >= start with the keyframes from \p other.
	 *
	 * @param other Curve that keyframes are copied from. Must have the same number of elements.
	 * @param start Start time at which keyframes are replaced.
	 */
	void sync(const Vector<T> &other, const time::time_t &start);

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
		if (this->_idstr.empty()) {
			return std::to_string(this->id());
		}
		return this->_idstr;
	}

private:
	/**
	 * Keyframe containers of the elements.
	 */
	container_t containers;

	/**
	 * Identifier for the container.
	 */
	const size_t _id;

	/**
	 * Human-readable identifier for the container.
	 */
	const std::string _idstr;

	/**
	 * Index of the last accessed keyframe of each element, used as search hint.
	 *
	 * mutable as hints are updated by const read-only functions.
	 */
	mutable std::vector<typename KeyframeContainer<T>::elem_ptr> last_elements;
};


template <typename T>
Vector<T>::Vector(const std::shared_ptr<event::EventLoop> &loop,
                  size_t id,
                  const std::vector<T> &default_vals,
                  const std::string &idstr,
                  const EventEntity::single_change_notifier &notifier) :
	EventEntity(loop, notifier),
	containers{},
	_id{id},
	_idstr{idstr},
	last_elements(default_vals.size(), 0) {
	this->containers.reserve(default_vals.size());
	for (const auto &val : default_vals) {
		this->containers.emplace_back(val);
	}
}

template <typename T>
T Vector<T>::at(const time::time_t &t, index_t index) const {
	return this->frame(t, index).second;
}

template <typename T>
std::vector<T> Vector<T>::get(const time::time_t &t) const {
	std::vector<T> values;
	values.reserve(this->containers.size());
	for (index_t i = 0; i < this->containers.size(); ++i) {
		values.push_back(this->at(t, i));
	}
	return values;
}

template <typename T>
size_t Vector<T>::size() const {
	return this->containers.size();
}

template <typename T>
std::pair<time::time_t, T> Vector<T>::frame(const time::time_t &t, index_t index) const {
	const auto &container = this->containers.at(index);
	auto e = container.last(t, this->last_elements[index]);
	this->last_elements[index] = e;

	return container.get(e).as_pair();
}

template <typename T>
std::pair<time::time_t, T> Vector<T>::next_frame(const time::time_t &t, index_t index) const {
	const auto &container = this->containers.at(index);
	auto e = container.last(t, this->last_elements[index]);
	this->last_elements[index] = e;

	ENSURE(e + 1 < container.size(),
	       "element " << index << " has no keyframe after t=" << t);

	return container.get(e + 1).as_pair();
}

template <typename T>
void Vector<T>::set_insert(const time::time_t &t, index_t index, const T &value) {
	auto &container = this->containers.at(index);
	this->last_elements[index] = container.insert_after(Keyframe{t, value}, this->last_elements[index]);

	this->changes(t);
}

template <typename T>
void Vector<T>::set_last(const time::time_t &t, index_t index, const T &value) {
	auto &container = this->containers.at(index);
	auto e = container.last(t, this->last_elements[index]);

	// erase max one same-time value, but never the default keyframe at -INF
	if (e > 0 and container.get(e).time() == t) {
		--e;
	}

	// all remaining keyframes have time <= t, so the new one is appended to
	// become the last keyframe at t
	e = container.erase_after(e);
	this->last_elements[index] = container.insert_after(Keyframe{t, value}, e);

	this->changes(t);
}

template <typename T>
void Vector<T>::set_replace(const time::time_t &t, index_t index, const T &value) {
	auto &container = this->containers.at(index);
	this->last_elements[index] = container.insert_overwrite(Keyframe{t, value}, this->last_elements[index]);

	this->changes(t);
}

template <typename T>
void Vector<T>::sync(const Vector<T> &other, const time::time_t &start) {
	ENSURE(other.size() == this->size(),
	       "cannot sync vector curves of different sizes: " << other.size()
	                                                        << "; expected: " << this->size());

	for (index_t i = 0; i < this->containers.size(); ++i) {
		this->containers[i].sync(other.containers[i], start);
		this->last_elements[i] = 0;
	}

	this->changes(start);
}

} // namespace curve
} // namespace openage
