// Copyright 2024-2025 the openage authors. See copying.md for legal info.

#pragma once

#include <array>

#include "curve/iterator.h"
#include "curve/keyframe_container.h"
#include "event/evententity.h"


namespace openage {
namespace curve {

template <typename T, size_t Size>
class Array : event::EventEntity {
public:
	/**
	 * The underlaying container type.
	 */
	using container_t = std::array<KeyframeContainer<T>, Size>;

	Array(const std::shared_ptr<event::EventLoop> &loop,
	      size_t id,
	      const T &default_val = T(),
	      const std::string &idstr = "",
	      const EventEntity::single_change_notifier &notifier = nullptr) :
		EventEntity(loop, notifier),
		_id{id}, _idstr{idstr}, loop{loop}, container{KeyframeContainer(default_val)} {
	}

	Array(const Array &) = delete;


	/**
	 * Get the last element with elem->time <= time from
	 * the keyfram container at a given index
	 */
	T at(const time::time_t &t, const size_t index) const;


	/**
	 * Get an array of the last elements with elem->time <= time from
	 * all keyfram containers contained within this array curve
	 */
	std::array<T, Size> get(const time::time_t &t) const;

	/**
	 * Get the amount of KeyframeContainers in array curve
	 */
	consteval size_t size() const;

	/**
	 * Get the last element and associated time which has elem->time <= time from
	 * the keyfram container at a given index
	 */
	std::pair<time::time_t, T> frame(const time::time_t &t, const size_t index) const;


	/**
	 * Get the first element and associated time which has elem->time >= time from
	 * the keyfram container at a given index
	 */
	std::pair<time::time_t, T> next_frame(const time::time_t &t, const size_t index) const;


	/**
	 * Insert a new keyframe value at time t at index
	 */
	void set_insert(const time::time_t &t, const size_t index, T value);


	/**
	 * Insert a new keyframe value at time t; delete all keyframes after time t at index
	 */
	void set_last(const time::time_t &t, const size_t index, T value);


	/**
	 * Insert a new keyframe value at time t at index i; remove all other keyframes with time t at index i
	 */
	void set_replace(const time::time_t &t, const size_t index, T value);


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


	/**
	 *  Array::Iterator is used to iterate over KeyframeContainers contained in a curve at a given time.
	 */
	class Iterator {
	public:
		Iterator(Array<T, Size> *curve, const time::time_t &time = time::TIME_MAX, size_t offset = 0) :
			curve(curve), time(time), offset(offset) {};

		/**
		 *  returns a copy of the keyframe at the current offset and time
		 */
		const T operator*() {
			return this->curve->frame(this->time, this->offset).second;
		}

		/**
		 * increments the Iterator to point at the next KeyframeContainer
		 */
		void operator++() {
			this->offset++;
		}

		/**
		 * Compare two Iterators by their offset
		 */
		bool operator!=(const Array<T, Size>::Iterator &rhs) const {
			return this->offset != rhs.offset;
		}


	private:
		/**
		 * used to index the Curve::Array pointed to by this iterator
		 */
		size_t offset;

		/**
		 * the curve object that this iterator, iterates over
		 */
		Array<T, Size> *curve;

		/**
		 * time at which this iterator is iterating over
		 */
		time::time_t time;
	};


	/**
	 * iterator pointing to a keyframe of the first KeyframeContainer in the curve at a given time
	 */
	Iterator begin(const time::time_t &time = time::TIME_MIN);

	/**
	 *  iterator pointing after the last KeyframeContainer in the curve at a given time
	 */
	Iterator end(const time::time_t &time = time::TIME_MIN);


private:
	/**
	 * get a copy to the KeyframeContainer at index
	 */
	KeyframeContainer<T> operator[](size_t index) const {
		return this->container.at(index);
	}


	std::array<KeyframeContainer<T>, Size> container;

	/**
	 * hints for KeyframeContainer operations
	 * hints is used to speed up the search for next keyframe to be accessed
	 * mutable as hints are updated by const read-only functions.
	 */
	mutable std::array<size_t, Size> last_element = {};

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
};


template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::frame(const time::time_t &t, const size_t index) const {
	size_t &hint = this->last_element[index];
	size_t frame_index = this->container.at(index).last(t, hint);
	hint = frame_index;
	return this->container.at(index).get(frame_index).make_pair();
}

template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::next_frame(const time::time_t &t, const size_t index) const {
	size_t &hint = this->last_element[index];
	size_t frame_index = this->container.at(index).last(t, hint);
	hint = frame_index;
	return this->container.at(index).get(frame_index + 1).make_pair();
}

template <typename T, size_t Size>
T Array<T, Size>::at(const time::time_t &t, const size_t index) const {
	size_t &hint = this->last_element[index];
	size_t frame_index = this->container.at(index).last(t, hint);
	hint = frame_index;
	return this->container.at(index).get(frame_index).val();
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
void Array<T, Size>::set_insert(const time::time_t &t, const size_t index, T value) {
	this->last_element[index] = this->container.at(index).insert_after(Keyframe{t, value}, this->last_element[index]);
}


template <typename T, size_t Size>
void Array<T, Size>::set_last(const time::time_t &t, const size_t index, T value) {
	size_t frame_index = this->container.at(index).insert_after(Keyframe{t, value}, this->last_element[index]);
	this->last_element[index] = frame_index;
	this->container.at(index).erase_after(frame_index);
}


template <typename T, size_t Size>
void Array<T, Size>::set_replace(const time::time_t &t, const size_t index, T value) {
	this->container.at(index).insert_overwrite(Keyframe{t, value}, this->last_element[index]);
}

template <typename T, size_t Size>
void Array<T, Size>::sync(const Array<T, Size> &other, const time::time_t &start) {
	for (int i = 0; i < Size; i++) {
		this->container[i].sync(other.container[i], start);
	}
}

template <typename T, size_t Size>
typename Array<T, Size>::Iterator Array<T, Size>::begin(const time::time_t &time) {
	return Array<T, Size>::Iterator(this, time);
}


template <typename T, size_t Size>
typename Array<T, Size>::Iterator Array<T, Size>::end(const time::time_t &time) {
	return Array<T, Size>::Iterator(this, time, this->container.size());
}

} // namespace curve
} // namespace openage
