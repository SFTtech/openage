// Copyright 2024-2024 the openage authors. See copying.md for legal info.

#pragma once

#include <array>

#include "curve/base_curve.h"
#include "curve/keyframe_container.h"


// remember to update docs
namespace openage {
namespace curve {

template <typename T, size_t Size>
class Array {
public:
	Array() = default;

	// prevent accidental copy of queue
	Array(const Array &) = delete;


	T get(const time::time_t &t, const size_t index) const;


	std::array<T, Size> get_all(const time::time_t &t) const;


	consteval size_t size() const;

	std::pair<time::time_t, T> frame(const time::time_t &t, const size_t index) const;


	std::pair<time::time_t, T> next_frame(const time::time_t &t, const size_t index) const;

	void set_insert(const time::time_t &t, const size_t index, T value);

	void set_last(const time::time_t &t, const size_t index, T value);

	void set_replace(const time::time_t &t, const size_t index, T value);

	void sync(const Array<T, Size> &other, const time::time_t &t);


	class Iterator {
	public:
		Iterator(Array<T, Size> *curve, const time::time_t &time = time::TIME_MAX, size_t offset = 0) :
			curve(curve), time(time), offset(offset) {};

		const T &operator*() {
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
	mutable size_t last_element = 0;
};


template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::frame(const time::time_t &t, const size_t index) const {
	this->last_element = container[index].last(t, this->last_element);
	return container[index].get(this->last_element).make_pair();
}

template <typename T, size_t Size>
std::pair<time::time_t, T> Array<T, Size>::next_frame(const time::time_t &t, const size_t index) const {
	this->last_element = container[index].last(t, this->last_element);
	return container[index].get(this->last_element + 1);
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
	this->last_element = this->container[index].insert_after(Keyframe(t, value), this->last_element);
}


template <typename T, size_t Size>
void Array<T, Size>::set_last(const time::time_t &t, const size_t index, T value) {
	this->last_element = this->container[index].insert_after(Keyframe(t, value), this->last_element);
	this->container[index].erase_after(this->last_element);
}


template <typename T, size_t Size>
void Array<T, Size>::set_replace(const time::time_t &t, const size_t index, T value) {
	this->container[index].insert_overwrite(Keyframe(t, value), this->last_element);
}

template <typename T, size_t Size>
void Array<T, Size>::sync(const Array<T, Size> &other, const time::time_t &start) {
	for (int i = 0; i < Size; i++) {
		this->container[i].sync(other[i], start);
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
