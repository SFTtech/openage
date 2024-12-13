// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <array>

#include "curve/keyframe_container.h"
#include "event/evententity.h"


// remember to update docs
namespace openage {
namespace curve {

template <typename T, size_t Size>
class Array : event::EventEntity {
public:
	Array(const std::shared_ptr<event::EventLoop> &loop,
	          size_t id,
	          const std::string &idstr = "",
	          const EventEntity::single_change_notifier &notifier = nullptr) :
		EventEntity(loop, notifier), _id{id}, _idstr{idstr}, loop{loop}{}

	Array(const Array &) = delete;


	/**
	 * Get the last element with elem->time <= time from
	 * the keyfram container at a given index
	 */
	T get(const time::time_t &t, const size_t index) const;


	/**
	 * Get an array of the last elements with elem->time <= time from
	 * all keyfram containers contained within this array curve
	 */
	std::array<T, Size> get_all(const time::time_t &t) const;


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


	KeyframeContainer<T>& operator[] (size_t index)
	{
		return this->container[index];
	}

	KeyframeContainer<T> operator[] (size_t index) const
	{
		return this->container[index];
	}

	class Iterator {
	public:
		Iterator(Array<T, Size> *curve, const time::time_t &time = time::TIME_MAX, size_t offset = 0) :
			curve(curve), time(time), offset(offset) {};

		const T operator*() {
			return curve->frame(this->time, this->offset).second;
		}

		void operator++() {
			this->offset++;
		}

		bool operator!=(const Array<T, Size>::Iterator &rhs) const {
			return this->offset != rhs.offset;
		}


	private:
		size_t offset;
		Array<T, Size> *curve;
		time::time_t time;
	};


	Iterator begin(const time::time_t &time = time::TIME_MAX);

	Iterator end(const time::time_t &time = time::TIME_MAX);


private:
	std::array<KeyframeContainer<T>, Size> container;

	//hint for KeyframeContainer operations
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
	size_t frmae_index = container[index].last(t, this->last_element[index]);
	this->last_element[index] = frmae_index;
	return container[index].get(frmae_index).make_pair();
}

template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::next_frame(const time::time_t &t, const size_t index) const {
	size_t frmae_index = container[index].last(t, this->last_element[index]);
	this->last_element[index] = frmae_index;
	return container[index].get(frmae_index + 1).make_pair();
}

template <typename T, size_t Size>
T Array<T, Size>::get(const time::time_t &t, const size_t index) const {
	return this->frame(t, index).second;
}

template <typename T, size_t Size>
std::array<T, Size> Array<T, Size>::get_all(const time::time_t &t) const {
	return [&]<auto... I>(std::index_sequence<I...>) {
		return std::array<T, Size>{this->get(t, I)...};
	}(std::make_index_sequence<Size>{});
}

template <typename T, size_t Size>
consteval size_t Array<T, Size>::size() const {
	return Size;
}


template <typename T, size_t Size>
void Array<T, Size>::set_insert(const time::time_t &t, const size_t index, T value) {
	this->last_element[index] = this->container[index].insert_after(Keyframe(t, value), this->last_element[index]);
}


template <typename T, size_t Size>
void Array<T, Size>::set_last(const time::time_t &t, const size_t index, T value) {

	size_t frame_index = this->container[index].insert_after(Keyframe(t, value), this->last_element[index]);
	this->last_element[index] = frame_index;
	this->container[index].erase_after(frame_index);
}


template <typename T, size_t Size>
void Array<T, Size>::set_replace(const time::time_t &t, const size_t index, T value) {
	this->container[index].insert_overwrite(Keyframe(t, value), this->last_element[index]);
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
