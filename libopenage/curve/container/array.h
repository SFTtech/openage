// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <array>

#include "curve/container/iterator.h"
#include "curve/keyframe_container.h"
#include "event/evententity.h"


namespace openage {
namespace curve {

template <typename T, size_t Size>
constexpr std::array<KeyframeContainer<T>, Size> init_default_vals(const std::array<T, Size> &default_vals) {
	std::array<KeyframeContainer<T>, Size> containers;
	for (size_t i = 0; i < Size; i++) {
		containers[i] = KeyframeContainer<T>(default_vals[i]);
	}
	return containers;
}

template <typename T, size_t Size>
class Array : event::EventEntity {
public:
	/// Underlying container type.
	using container_t = std::array<KeyframeContainer<T>, Size>;

	/// Uderlying iterator type
	using const_iterator = typename std::array<KeyframeContainer<T>, Size>::const_iterator;

	/// Index type to access elements in the container.
	using index_t = typename container_t::size_type;

	/**
	 * Create a new array curve container.
	 *
	 * @param loop Event loop this curve is registered on for notifications.
	 * @param id Unique identifier for this curve.
	 * @param idstr Human-readable identifier for this curve.
	 * @param notifier Function to call when this curve changes.
	 * @param default_vals Default values for the array elements.
	 */
	Array(const std::shared_ptr<event::EventLoop> &loop,
	      size_t id,
	      const std::string &idstr = "",
	      const EventEntity::single_change_notifier &notifier = nullptr,
	      const std::array<T, Size> &default_vals = {}) :
		EventEntity(loop, notifier),
		containers{init_default_vals(default_vals)},
		_id{id},
		_idstr{idstr},
		loop{loop},
		last_elements{} {}

	// prevent copying because it invalidates the usage of unique ids and event
	// registration. If you need to copy a curve, use the sync() method.
	Array(const Array &) = delete;
	Array &operator=(const Array &) = delete;

	Array(Array &&) = default;

	/**
	 * Get the last element with elem->time <= time.
	 *
	 * @param t Time of access.
	 * @param index Index of the array element.
	 *
	 * @return Value of the last element with time <= t.
	 */
	T at(const time::time_t &t, const index_t index) const;

	/**
	 * Get all elements at time t.
	 *
	 * @param t Time of access.
	 *
	 * @return Array of values at time t.
	 */
	std::array<T, Size> get(const time::time_t &t) const;

	/**
	 * Get the size of the array.
	 *
	 * @return Array size.
	 */
	consteval size_t size() const;

	/**
	 * Get the last keyframe value and time with elem->time <= time.
	 *
	 * @param t Time of access.
	 * @param index Index of the array element.
	 *
	 * @return Time-value pair of the last keyframe with time <= t.
	 */
	std::pair<time::time_t, T> frame(const time::time_t &t, const index_t index) const;

	/**
	 * Get the first keyframe value and time with elem->time > time.
	 *
	 * If there is no keyframe with time > t, the behavior is undefined.
	 *
	 * @param t Time of access.
	 * @param index Index of the array element.
	 *
	 * @return Time-value pair of the first keyframe with time > t.
	 */
	std::pair<time::time_t, T> next_frame(const time::time_t &t, const index_t index) const;

	/**
	 * Insert a new keyframe value at time t.
	 *
	 * If there is already a keyframe at time t, the new keyframe is inserted after the existing one.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the array element.
	 * @param value Keyframe value.
	 */
	void set_insert(const time::time_t &t, const index_t index, T value);

	/**
	 * Insert a new keyframe value at time t. Erase all other keyframes with elem->time > t.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the array element.
	 * @param value Keyframe value.
	 */
	void set_last(const time::time_t &t, const index_t index, T value);

	/**
	 * Replace all keyframes at elem->time == t with a new keyframe value.
	 *
	 * @param t Time of insertion.
	 * @param index Index of the array element.
	 * @param value Keyframe value.
	 */
	void set_replace(const time::time_t &t, const index_t index, T value);

	/**
	 * Copy keyframes from another container to this container.
	 *
	 * Replaces all keyframes beginning at t >= start with keyframes from \p other.
	 *
	 * @param other Curve that keyframes are copied from.
	 * @param start Start time at which keyframes are replaced (default = -INF).
	 *              Using the default value replaces ALL keyframes of \p this with
	 *              the keyframes of \p other.
	 */
	void sync(const Array<T, Size> &other, const time::time_t &start);

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


	class ArrayIterator : public CurveIterator<T, Array<T, Size>> {
	public:
		/**
		 * Construct the iterator from its boundary conditions: time and container
		 */
		ArrayIterator(const const_iterator &base,
		              const Array<T, Size> *base_container,
		              const time::time_t &to) :
			CurveIterator<T, Array<T, Size>>(base, base_container, -time::TIME_MAX, to) {
		}


		virtual bool valid() const override {
			if (this->container->end().get_base() != this->get_base() && this->get_base()->begin()->time() <= this->to) {
				return true;
			}
			return false;
		}

		/**
		 * Get the keyFrame with a time <= this->to from the KeyframeContainer
		 * that this iterator currently points at
		 */
		const T &value() const override {
			const auto &key_frame_container = *this->get_base();
			size_t hint_index = std::distance(this->container->begin().get_base(), this->get_base());
			size_t e = key_frame_container.last(this->to, last_elements[hint_index]);
			this->last_elements[hint_index] = e;
			return key_frame_container.get(e).val();
		}

		/**
		 * Cache hints for containers. Stores the index of the last keyframe accessed in each container.
		 *
		 * hints is used to speed up the search for keyframes.
		 *
		 * mutable as hints are updated by const read-only functions.
		 */
		mutable std::array<typename KeyframeContainer<T>::elem_ptr, Size> last_elements = {};
	};


	/**
	 * iterator pointing to a keyframe of the first KeyframeContainer in the curve at a given time
	 */
	ArrayIterator begin(const time::time_t &time = time::TIME_MIN) const;


	/**
	 *  iterator pointing after the last KeyframeContainer in the curve at a given time
	 */
	ArrayIterator end(const time::time_t &time = time::TIME_MAX) const;


private:
	/**
	 * get a copy to the KeyframeContainer at index
	 */
	KeyframeContainer<T> operator[](index_t index) const {
		return this->containers.at(index);
	}

	/**
	 * Containers for each array element.
	 *
	 * Each element is managed by a KeyframeContainer.
	 */
	container_t containers;

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
	 * Cache hints for containers. Stores the index of the last keyframe accessed in each container.
	 *
	 * hints is used to speed up the search for keyframes.
	 *
	 * mutable as hints are updated by const read-only functions.
	 */
	mutable std::array<typename KeyframeContainer<T>::elem_ptr, Size> last_elements = {};
};


template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::frame(const time::time_t &t,
                                                 const index_t index) const {
	// find elem_ptr in container to get the last keyframe
	auto hint = this->last_elements[index];
	auto frame_index = this->containers.at(index).last(t, hint);

	// update the hint
	this->last_elements[index] = frame_index;

	return this->containers.at(index).get(frame_index).as_pair();
}

template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::next_frame(const time::time_t &t,
                                                      const index_t index) const {
	// find elem_ptr in container to get the last keyframe with time <= t
	auto hint = this->last_elements[index];
	auto frame_index = this->containers.at(index).last(t, hint);

	// increment the index to get the next keyframe
	frame_index++;

	// update the hint
	this->last_elements[index] = frame_index;

	return this->containers.at(index).get(frame_index).as_pair();
}

template <typename T, size_t Size>
T Array<T, Size>::at(const time::time_t &t,
                     const index_t index) const {
	// find elem_ptr in container to get the last keyframe with time <= t
	auto hint = this->last_elements[index];
	auto e = this->containers.at(index).last(t, hint);

	// update the hint
	this->last_elements[index] = e;

	return this->containers.at(index).get(e).val();
}

template <typename T, size_t Size>
std::array<T, Size> Array<T, Size>::get(const time::time_t &t) const {
	return [&]<auto... I>(std::index_sequence<I...>) {
		return std::array<T, Size>{this->at(t, I)...};
	}(std::make_index_sequence<Size>{});
}

template <typename T, size_t Size>
consteval size_t Array<T, Size>::size() const {
	return Size;
}

template <typename T, size_t Size>
void Array<T, Size>::set_insert(const time::time_t &t,
                                const index_t index,
                                T value) {
	// find elem_ptr in container to get the last keyframe with time <= t
	auto hint = this->last_elements[index];
	auto e = this->containers.at(index).insert_after(Keyframe{t, value}, hint);

	// update the hint
	this->last_elements[index] = e;

	this->changes(t);
}

template <typename T, size_t Size>
void Array<T, Size>::set_last(const time::time_t &t,
                              const index_t index,
                              T value) {
	// find elem_ptr in container to get the last keyframe with time <= t
	auto hint = this->last_elements[index];
	auto e = this->containers.at(index).last(t, hint);

	// erase max one same-time value
	if (this->containers.at(index).get(e).time() == t) {
		e--;
	}

	// erase all keyframes with time > t
	this->containers.at(index).erase_after(e);

	// insert the new keyframe at the end
	this->containers.at(index).insert_before(Keyframe{t, value}, e);

	// update the hint
	this->last_elements[index] = hint;

	this->changes(t);
}

template <typename T, size_t Size>
void Array<T, Size>::set_replace(const time::time_t &t,
                                 const index_t index,
                                 T value) {
	// find elem_ptr in container to get the last keyframe with time <= t
	auto hint = this->last_elements[index];
	auto e = this->containers.at(index).insert_overwrite(Keyframe{t, value}, hint);

	// update the hint
	this->last_elements[index] = e;

	this->changes(t);
}

template <typename T, size_t Size>
void Array<T, Size>::sync(const Array<T, Size> &other,
                          const time::time_t &start) {
	for (index_t i = 0; i < Size; i++) {
		this->containers[i].sync(other.containers[i], start);
	}

	this->changes(start);
}

template <typename T, size_t Size>
auto Array<T, Size>::begin(const time::time_t &t) const -> ArrayIterator {
	return ArrayIterator(this->containers.begin(), this, t);
}


template <typename T, size_t Size>
auto Array<T, Size>::end(const time::time_t &t) const -> ArrayIterator {
	return ArrayIterator(this->containers.end(), this, t);
}

} // namespace curve
} // namespace openage
